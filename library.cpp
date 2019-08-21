#include "library.h"

#include <iostream>
#include <stdio.h>

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

void changeStr(std::string& str)
{
    str += " changed";
}

