// #include "test_module.h"
#include <exception>
#include <iostream>
// Exeptions.
int main() {
    std::cout << "Hello Template Project \n";
    throw std::out_of_range("exception");
    return 0;
}