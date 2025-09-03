#include "test_module.h"
#include <iostream>


namespace my_code{
    void namespace_print(int a, int b){
        std::cout << "the two values are a: " << a << " " << "b: " << b << std::endl;
    }
};

// Enum, Oridinary C type.
enum Car {
    BMW,
    Mercedes,
    KIA
};

// New Enum style in Cpp and recommended.
enum class Cars : long { // we can also identify the integer type (unsigned int, long, ....)
    Porcha,
    VW,
    BYD
};


int main(){
    // multiple files compilation.
    my_print();

    // namespacing.
    my_code::namespace_print(10, 20);

    // casting.
    // static cast.
    int a = 10;
    int b = 3;

    float i = static_cast<float>(a) / b;

    std::cout << "$i after static cast: " << i << std::endl;

    return 0;
}