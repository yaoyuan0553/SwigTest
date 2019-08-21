#ifndef SWIGTEST_LIBRARY_H
#define SWIGTEST_LIBRARY_H

#include <string>

void hello();

template <typename T1, typename T2>
struct Pair {
    T1 first;
    T2 second;
};

void printStr(const char* str);

std::string makeStr();

void changeStr(std::string& str);

#endif //SWIGTEST_LIBRARY_H