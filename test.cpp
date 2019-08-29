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
#include <mutex>

#include "Utility.h"
#include "Thread.h"
#include "Semaphore.h"
#include "ThreadPool.h"
#include "StaticQueue.h"
#include "ConcurrentStaticQueue.h"

using namespace std;


constexpr int NUM_CONSUMERS = 100;
constexpr int N_Q_CYCLES = 5;
constexpr int N_SEM_RELEASE  = 100;
constexpr int TOTAL_TASKS = N_Q_CYCLES * N_SEM_RELEASE;

pthread_mutex_t m;
atomic_bool quit;
int tasksProcessed = 0;
thread_local int ti = 0;

struct PCSems {
    sem_t &produced, &consumed;
};

void* consumerRun(void* args)
{
    PCSems& pcSems = *(PCSems*)args;
    for (;;)
    {
        // wait on producer to produce
        if (sem_wait(&pcSems.produced))
            PERROR("sem_wait() produced");

        // quit on condition
        if (quit) {
            cout << this_thread::get_id() << ": quit signal received, exiting...\n";
            break;
        }

        // lock m
        if (pthread_mutex_lock(&m))
            PERROR("pthread_mutex_lock()");

        // consume
        cout << this_thread::get_id() << ": " << ti++ << '\n';
        ++tasksProcessed;

        // unlock m
        if (pthread_mutex_unlock(&m))
            PERROR("pthread_mutex_unlock()");

        // release producer
        if (sem_post(&pcSems.consumed))
            PERROR("sem_post() consumed");

    }
    pthread_exit(nullptr);
}

void* producerRun(void* args)
{

    PCSems& pcSems = *(PCSems*)args;

    for (int i = 0; i < N_Q_CYCLES; i++)
    {
        // release N_SEM_RELEASE times
        for (int j = 0; j < N_SEM_RELEASE; j++) {
            // wait on consumed
            if (sem_wait(&pcSems.consumed))
                PERROR("sem_wait() consumed");

            // produce
            // emulate producing
            this_thread::sleep_for(chrono::milliseconds(10));

            // release produced
            if (sem_post(&pcSems.produced))
                PERROR("sem_post()");
        }

        this_thread::sleep_for(chrono::seconds(1));
    }

    quit = true;

    // release consumer threads to quit
    for (int i = 0; i < NUM_CONSUMERS; i++)
        if (sem_post(&pcSems.produced))
            PERROR("sem_post()");

    pthread_exit(nullptr);
}

void testSem()
{
    // initialize semaphore
    sem_t produced, consumed;
    if (sem_init(&produced, 0, 0))
        PERROR("sem_init()");

    if (sem_init(&consumed, 0, NUM_CONSUMERS))
        PERROR("sem_init()");

    PCSems pcSems{.produced = produced, .consumed = consumed };

    // initialize m
    if (pthread_mutex_init(&m, nullptr))
        PERROR("pthread_mutex_init()");

    quit = false;

    pthread_t consumerThreads[NUM_CONSUMERS];

    pthread_t producerThread;

    // create threads
    for (pthread_t& pThread : consumerThreads) {
        if (pthread_create(&pThread, nullptr, &consumerRun, &pcSems))
            PERROR("pthread_create() thread");
    }

    // create quit thread
    if (pthread_create(&producerThread, nullptr, &producerRun, &pcSems))
        PERROR("pthread_create() producerThread");

    // join quit thread first
    if (pthread_join(producerThread, nullptr))
        PERROR("pthread_join() producerThread");

    // join threads
    for (pthread_t& pThread : consumerThreads) {
        if (pthread_join(pThread, nullptr))
            PERROR("pthread_join()");
    }

    printf("tasks processed: %d/%d\n", tasksProcessed, TOTAL_TASKS);

    if (sem_destroy(&produced))
        PERROR("sem_destroy() produced");

    if (sem_destroy(&consumed))
        PERROR("sem_destroy() consumed");

    if (pthread_mutex_destroy(&m))
        PERROR("pthread_mutex_destroy()");
}

class ConsumerThread : public Thread {

    Semaphore& full_;
    Semaphore& empty_;
    mutex& m_;

    void internalRun() final
    {
        for (;;)
        {
            full_.wait();

            if (quit) {
                cout << this_thread::get_id() << ": quit signal received, exiting...\n";
                break;
            }

            // lock mutex
            m_.lock();

            cout << this_thread::get_id() << ": " << ti++ << '\n';
            ++tasksProcessed;

            m_.unlock();

            empty_.release();
        }
    }
public:
    explicit ConsumerThread(Semaphore& full, Semaphore& empty, mutex& m) : full_(full), empty_(empty), m_(m) { }
};

class ProducerThread : public Thread {
    Semaphore& full_;
    Semaphore& empty_;
    mutex& m_;

    void internalRun() final
    {
        for (int i = 0; i < N_Q_CYCLES; i++)
        {
            for (int j = 0; j < N_SEM_RELEASE; j++) {

                empty_.wait();

                this_thread::sleep_for(chrono::milliseconds(10));
                m_.lock();
                m_.unlock();

                full_.release();
            }
            this_thread::sleep_for(chrono::seconds(1));
        }
        quit = true;
        full_.release(NUM_CONSUMERS);
    }

public:
    explicit ProducerThread(Semaphore& full, Semaphore& empty, mutex& m) : full_(full), empty_(empty), m_(m) { }
};


void testCustomThreadsAndSems()
{
    Semaphore full, empty(NUM_CONSUMERS);
    mutex m;

    ProducerThread p(full, empty, m);

    ThreadPool consumerThreads;
    consumerThreads.add<ConsumerThread>(NUM_CONSUMERS, full, empty, m);

    p.run();
    consumerThreads.runAll();

    p.wait();
    consumerThreads.waitAll();

    printf("tasks processed: %d/%d\n", tasksProcessed, TOTAL_TASKS);
}

class CSQueueConsumerThread : public Thread {

    CSQueue<int>& dataQueue_;

    void internalRun() final
    {
        for (;;)
        {
            auto [x, quit] = dataQueue_.pop();

            if (quit) break;

            cout << this_thread::get_id() << ": " << x << '\n';
//            this_thread::sleep_for(chrono::seconds(1));

        }
    }
public:
    explicit CSQueueConsumerThread(CSQueue<int>& dataQueue) : dataQueue_(dataQueue) { }
};

class CSQueueProducerThread : public Thread {
    CSQueue<int>& dataQueue_;

    void internalRun() final
    {
        for (int i = 0; i < TOTAL_TASKS; i++)
        {
//            this_thread::sleep_for(chrono::milliseconds(10));

            dataQueue_.push(i);
        }

        dataQueue_.setQuitSignal();
    }

public:
    explicit CSQueueProducerThread(CSQueue<int>& dataQueue) : dataQueue_(dataQueue) { }
};


void testCustomQueue()
{
    constexpr size_t MAX_SIZE = 100;
    constexpr int WRITE_AHEAD = 50;

    CSQueue<int> csQueue(MAX_SIZE, WRITE_AHEAD, NUM_CONSUMERS);

    CSQueueProducerThread producer(csQueue);
    ThreadPool consumers;
    consumers.add<CSQueueConsumerThread>(NUM_CONSUMERS, csQueue);

    producer.run();
    consumers.runAll();

    producer.wait();
    consumers.waitAll();
}


int main()
{
//    testSem();
//    testCustomThreadsAndSems();
    testCustomQueue();

    return 0;
}