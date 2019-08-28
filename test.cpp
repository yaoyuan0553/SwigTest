//
// Created by yuan on 8/26/19.
//
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>

#include <thread>
#include <iostream>
#include <vector>
#include <atomic>

using namespace std;

#define PERROR(x...)                                                    \
    do {                                                                \
        fprintf(stderr, "[-] ERROR: " x);                               \
		fprintf(stderr, "\n\tLocation : %s(), %s:%u\n", __FUNCTION__,   \
			__FILE__, __LINE__);                                        \
        exit(-1);                                                       \
    } while (0)


constexpr int NUM_THREADS = 10;
constexpr int N_Q_CYCLES = 5;
constexpr int N_SEM_RELEASE  = 3;
constexpr int TOTAL_TASKS = N_Q_CYCLES * N_SEM_RELEASE;

pthread_mutex_t mutex;
atomic_bool quit;
int tasksProcessed = 0;
thread_local int ti = 0;

void* threadRun(void* args)
{
    sem_t& sem = *(sem_t*)args;
    for (;;)
    {
        sem_wait(&sem);

        if (quit) {
            cout << this_thread::get_id() << ": quit signal received, exiting...\n";
            break;
        }

        pthread_mutex_lock(&mutex);
        cout << this_thread::get_id() << ": " << ti++ << '\n';
        ++tasksProcessed;
        this_thread::sleep_for(chrono::milliseconds(1200));
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(nullptr);
}

void* quitThreadRun(void* args)
{

    sem_t& sem = *(sem_t*)args;

    this_thread::sleep_for(chrono::seconds(1));
    for (int i = 0; i < N_Q_CYCLES; i++)
    {
        // release N_SEM_RELEASE times
        for (int j = 0; j < N_SEM_RELEASE; j++) {
            if (sem_post(&sem))
                PERROR("sem_post()");
        }

        this_thread::sleep_for(chrono::seconds(1));
    }

    quit = true;

    for (int i = 0; i < NUM_THREADS; i++)
        if (sem_post(&sem))
            PERROR("sem_post()");

    pthread_exit(nullptr);
}

void testSem()
{
    // initialize semaphore
    sem_t sem;
    if (sem_init(&sem, 0, 0))
        PERROR("sem_init()");

    // initialize mutex
    if (pthread_mutex_init(&mutex, nullptr))
        PERROR("pthread_mutex_init()");

    quit = false;

    pthread_t pThreads[NUM_THREADS];

    pthread_t quitThread;

    // create threads
    for (pthread_t& pThread : pThreads) {
        if (pthread_create(&pThread, nullptr, &threadRun, &sem))
            PERROR("pthread_create() thread");
    }

    // create quit thread
    if (pthread_create(&quitThread, nullptr, &quitThreadRun, &sem))
        PERROR("pthread_create() quitThread");

    // join quit thread first
    if (pthread_join(quitThread, nullptr))
        PERROR("pthread_join() quitThread");

    // join threads
    for (pthread_t& pThread : pThreads) {
        if (pthread_join(pThread, nullptr))
            PERROR("pthread_join()");
    }

    printf("tasks processed: %d/%d\n", tasksProcessed, TOTAL_TASKS);
}



int main()
{
    testSem();

    return 0;
}