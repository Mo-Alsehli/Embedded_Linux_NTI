# C++ Inheritance – Comprehensive Guide

---

## 1. What is Inheritance?

Inheritance allows a class (**derived/child**) to reuse and extend the properties and behavior of another class (**base/parent**).
It promotes **code reusability** and supports **polymorphism**.

---

## 2. Basic Syntax

```cpp
class Base {
public:
    void show() { std::cout << "Base\n"; }
};

class Derived : public Base {
public:
    void display() { std::cout << "Derived\n"; }
};

int main() {
    Derived d;
    d.show();    // inherited from Base
    d.display(); // defined in Derived
}
```

---

## 3. Types of Inheritance

### Public Inheritance

```cpp
class Derived : public Base {};
```

* Public → stays public
* Protected → stays protected
* Private → inaccessible

### Protected Inheritance

```cpp
class Derived : protected Base {};
```

* Public → becomes protected
* Protected → stays protected
* Private → inaccessible

### Private Inheritance

```cpp
class Derived : private Base {};
```

* Public & Protected → become private
* Private → inaccessible

---

## 4. Access Specifiers and Inheritance

| Base Member | Public Derivation | Protected Derivation | Private Derivation |
| ----------- | ----------------- | -------------------- | ------------------ |
| Public      | Public            | Protected            | Private            |
| Protected   | Protected         | Protected            | Private            |
| Private     | Not Inherited     | Not Inherited        | Not Inherited      |

---

## 5. Constructor & Destructor Rules

* **Base constructor** runs first, then derived.
* **Destructors** run in reverse order.

```cpp
class Base {
public:
    Base() { std::cout << "Base ctor\n"; }
    ~Base() { std::cout << "Base dtor\n"; }
};

class Derived : public Base {
public:
    Derived() { std::cout << "Derived ctor\n"; }
    ~Derived() { std::cout << "Derived dtor\n"; }
};

int main() {
    Derived d;
}
// Output:
// Base ctor
// Derived ctor
// Derived dtor
// Base dtor
```

---

## 6. Function Overriding

Derived can redefine base class functions.

```cpp
class Base {
public:
    void show() { std::cout << "Base\n"; }
};

class Derived : public Base {
public:
    void show() { std::cout << "Derived\n"; }
};
```

---

## 7. Virtual Functions (Polymorphism)

To enable **runtime polymorphism**.

```cpp
class Base {
public:
    virtual void show() { std::cout << "Base\n"; }
};

class Derived : public Base {
public:
    void show() override { std::cout << "Derived\n"; }
};

int main() {
    Base* b = new Derived();
    b->show(); // Derived
}
```

---

## 8. Abstract Classes & Pure Virtual Functions

Abstract class = at least one **pure virtual function**.

```cpp
class Shape {
public:
    virtual void draw() = 0;  // pure virtual
};

class Circle : public Shape {
public:
    void draw() override { std::cout << "Circle\n"; }
};
```

---

## 9. Multiple Inheritance

C++ allows a class to inherit from multiple classes.

```cpp
class A { public: void showA() {} };
class B { public: void showB() {} };

class C : public A, public B {};
```

---

## 10. Diamond Problem & Virtual Inheritance

```cpp
class A { public: int x; };
class B : public A {};
class C : public A {};
class D : public B, public C {}; // D has two copies of A
```

Solution: **virtual inheritance**

```cpp
class A { public: int x; };
class B : virtual public A {};
class C : virtual public A {};
class D : public B, public C {}; // only one A
```

---

## 11. `using` in Inheritance

Bring base functions into derived scope.

```cpp
class Base {
public:
    void show(int) {}
};

class Derived : public Base {
public:
    using Base::show; // expose all overloads
    void show(double) {}
};
```

---

## 12. `final` and `override` Keywords

* `override`: explicitly marks overridden function.
* `final`: prevents further overriding or inheritance.

```cpp
class Base {
    virtual void func();
};

class Derived final : public Base { // no further inheritance
    void func() override final;     // cannot override further
};
```

---

## 13. Object Slicing

When assigning a derived object to a base object, derived parts are **sliced off**.

```cpp
class Base { int x; };
class Derived : public Base { int y; };

Base b = Derived(); // y is sliced away
```

---

## ✅ Summary

* Inheritance allows code reuse and polymorphism.
* Types: public, protected, private.
* Use **virtual functions** for runtime polymorphism.
* Use **pure virtual functions** for abstract classes.
* Be aware of the **diamond problem** and solve it with virtual inheritance.
* Use `override` and `final` for safer polymorphism.
* Watch out for object slicing.
