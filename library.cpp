#include "library.h"

#include <iostream>
#include <stdio.h>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <mutex>

#include <pthread.h>
#include <semaphore.h>

using namespace std;


void hello()
{
    std::cout << "Hello, World!" << std::endl;
}

void printStr(const char* str)
{
    printf("%s\n", str);
}

std::string makeStr()
{
    return "made this string";
}

void takeStr(std::string str)
{
    std::cout << str << '\n';
}

void changeStr(char* str, int length)
{
    cout << str << " " << length << '\n';

    for (int i = 0; i < length; i++) {
        str[i] = 'a';
    }

    printf("changed str: %s\n", str);
}

void fillArray(int arr[], int n)
{
    for (int i = 0; i < n; i++) {
        arr[i] += i;
    }
}

void changeIntPointerArray(int* array, int n)
{
    for (int i = 0; i < n; i++)
        array[i] = i * 2;
}

uint64_t fib(uint64_t i)
{
    if (i < 1) return 0;
    if (i == 1) return 1;
    if (i == 2) return 2;

    return fib(i-1) + fib(i-2);
}

void printFib(int i)
{
    printf("fib: %lu\n", fib(i));
}

vector<int> testVec = {1,2,3,4,5,6,7,8,9,10};

vector<thread> tVec;

thread* createNewThread(int i)
{
    return new thread(printFib, i);
}

void waitThread(std::thread* t)
{
    t->join();
}

mutex m;
condition_variable cv;
int i = 0;
int nReleased = 0;

void printNum()
{
    for (;;)
    {
        unique_lock<mutex> lk(m);

        cerr << "waiting...\n";

        cv.wait(lk, [] { return nReleased > 0; });
        nReleased--;

        if (i > testVec.size()) {
            printf("index %d: out of range\n", i);
            return;
        }
        printf("index %d: %d\n", i, testVec[i]);
        i++;
        m.unlock();
    }
}

void initThreads(int numThreads)
{
    for (int i = 0; i < numThreads; i++)
        tVec.emplace_back(printNum);
}

void getResult(int n)
{
    m.lock();
    nReleased += n;
    m.unlock();

    cv.notify_all();
}
