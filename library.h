#ifndef SWIGTEST_LIBRARY_H
#define SWIGTEST_LIBRARY_H

#include <string>
#include <thread>

void hello();

template <typename T1, typename T2>
struct Pair {
    T1 first;
    T2 second;
};

void printStr(const char* str);

std::string makeStr();

void takeStr(std::string str);

void changeStr(char* str, int length);

void fillArray(int arr[], int n);

void changeIntPointerArray(int* array, int n);

std::thread* createNewThread(int i);

void waitThread(std::thread* t);

void initThreads(int numThreads);

void getResult(int n);


#endif //SWIGTEST_LIBRARY_H