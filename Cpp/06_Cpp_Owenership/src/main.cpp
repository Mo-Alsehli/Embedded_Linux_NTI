#include <iostream>
#include <memory>

#include "array.h"

class A {
   public:
    A(int n = 3) : m_num(n) { std::cout << "Calling Constructor\n"; }

    ~A() { std::cout << "Calling Destructor\n"; }

    void print() { std::cout << "Value: " << m_num << std::endl; }

   private:
    int m_num;
};

std::unique_ptr<A> foo() {
    std::cout << "Foo called" << std::endl;
    A* i = new A{5};
    std::unique_ptr<A> p{i};
    p->print();
    std::cout << "End of Foo" << std::endl;
    return p;
}

int main() {
    std::unique_ptr<A> a = foo();
    a->print();
    return 0;
}