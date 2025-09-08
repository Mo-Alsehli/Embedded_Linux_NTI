# C++ Templates – Comprehensive Guide

---

## 1. What are Templates?

Templates allow writing **generic code** that works with any type.
Instead of duplicating code for `int`, `double`, `string`, etc., you write a single **blueprint**.

---

## 2. Function Templates

```cpp
template <typename T>
T add(T a, T b) {
    return a + b;
}

int main() {
    add(3, 4);     // int
    add(2.5, 3.1); // double
}
```

* `typename T` introduces a type parameter.
* Compiler generates code for each type when needed.

---

## 3. Class Templates

```cpp
template <typename T>
class Box {
    T value;
public:
    Box(T v) : value(v) {}
    T get() { return value; }
};

Box<int> b1(10);
Box<std::string> b2("Hi");
```

---

## 4. Template Specialization

### Full

```cpp
template <typename T>
class Printer {
public:
    void print(T v) { std::cout << v; }
};

template <>
class Printer<char*> {
public:
    void print(char* v) { std::cout << "C-string: " << v; }
};
```

### Partial

```cpp
template <typename T, typename U>
class Pair {};

template <typename T>
class Pair<T, T> {};
```

---

## 5. Non-Type Template Parameters

```cpp
template <typename T, int Size>
class Array {
    T data[Size];
public:
    int getSize() { return Size; }
};

Array<int, 10> arr;
```

---

## 6. Default Template Arguments

```cpp
template <typename T = int, typename U = double>
class Example {
    T a;
    U b;
};
```

---

## 7. Template Functions in Classes

```cpp
class Math {
public:
    template <typename T>
    static T square(T x) { return x * x; }
};

Math::square(5);
Math::square(3.14);
```

---

## 8. Templates and Inheritance

```cpp
template <typename T>
class Base {};

template <typename T>
class Derived : public Base<T> {};
```

---

## 9. Variadic Templates

```cpp
template <typename... Args>
void print(Args... args) {
    (std::cout << ... << args) << "\n";
}

print(1, " hi ", 3.14);
```

---

## 10. Template Metaprogramming

```cpp
template <int N>
struct Factorial {
    static const int value = N * Factorial<N-1>::value;
};

template <>
struct Factorial<0> {
    static const int value = 1;
};

int x = Factorial<5>::value; // 120
```

---

## 11. Constraints (C++20 Concepts)

```cpp
#include <concepts>

template <std::integral T>
T add(T a, T b) { return a + b; }

add(3, 4);   // ok
add(3.2, 4); // error
```

---

## 12. Key Terms

* **Instantiation:** Compiler generates code for a specific type.
* **Specialization:** Custom behavior for specific types.
* **SFINAE:** Substitution Failure Is Not An Error.
* **Concepts:** C++20 way to constrain templates.

---

## 13. Real Uses

* STL containers: `std::vector<T>`
* STL algorithms: `std::sort`, `std::find`
* Smart pointers: `std::unique_ptr<T>`
* Type traits: `std::is_integral<T>`

---

## ✅ Summary

* Templates = generic programming in C++.
* Function/Class templates avoid duplication.
* Specialization allows customization.
* Non-type parameters allow compile-time constants.
* Variadic templates handle variable arguments.
* Concepts give modern constraints.
* Foundation of STL and modern C++ libraries.
