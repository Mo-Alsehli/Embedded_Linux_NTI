# CPP Recap Session Notes.

## **Namespacing in C++**

In C++, a *namespace* is used to **organize code** and **avoid name conflicts** between variables, functions, or classes that may have the same name in different parts of a program or in different libraries.

---

### 🔧 Why it’s needed

Imagine two libraries both define a function called `print()`.
If you include both libraries in your program, the compiler won’t know which `print()` you mean → **name collision**.

---

### ✅ How namespaces solve that

By grouping things inside a namespace, you make the full name unique.

```cpp
namespace LibraryA {
    void print(){
        // ...
    }
}

namespace LibraryB {
    void print(){
        // ...
    }
}
```

Then in your code, you specify which one you want:

```cpp
LibraryA::print();
LibraryB::print();
```

The `::` is called the **scope resolution operator**.

---

### 🧠 Common Usage: `std`

All standard C++ functions and classes are defined inside the `std` namespace.

Example:

```cpp
#include <iostream>

int main(){
    std::cout << "Hello\n";
    return 0;
}
```

Without the `std::`, the compiler won’t find `cout`.

You *can* bring everything from a namespace into the current scope using:

```cpp
using namespace std;
```

but this is generally **not recommended** in large projects because it can cause name collisions.

---

### ⚡ Summary

| Concept           | Meaning                                  |
| ----------------- | ---------------------------------------- |
| `namespace`       | Grouping of identifiers                  |
| `::`              | Scope resolution operator                |
| `using namespace` | Import full namespace into current scope |
| `std`             | Namespace of the C++ Standard Library    |

---

### `static_cast` in C++

`static_cast` is a C++ cast used for **compile-time type conversion**. It’s safe and explicit, and should be used when you want to convert between compatible types (e.g., `int` → `double`, pointer upcasting/downcasting).

---

#### ✅ Examples

```cpp
double d = 3.14;
int i = static_cast<int>(d);  // i = 3
```

```cpp
class Base {};
class Derived : public Base {};

Derived d_obj;
Base* bptr = static_cast<Base*>(&d_obj);  // upcast
```

---

#### ⚠️ Key Points

* Performed at **compile time**.
* **Explicit** (safer than C-style cast).
* Works only for **related** or **compatible** types.
* Does **not** check types at runtime (no safety for invalid downcasts).

---

#### ✔️ Use it when

* Converting between numeric types.
* Performing safe pointer upcasts.
* Converting enumerations to integers and vice versa.

---

#### ❌ Avoid it for

* Polymorphic downcasts → use `dynamic_cast` instead.
---

### Enums in C++

An **enum** (enumeration) is a user-defined type that consists of a set of named integral constants. It improves readability and makes code more meaningful.

---

#### ✅ Basic Syntax

```cpp
enum Color {
    Red,
    Green,
    Blue
};
```

* By default, the underlying type is `int`.
* Values start from `0` and increase automatically (`Red=0`, `Green=1`, `Blue=2`).

---

#### 🧠 Usage

```cpp
Color c = Green;

if (c == Green) {
    // do something
}
```

---

#### 🔢 Custom Values

```cpp
enum Status {
    Pending = 1,
    InProgress = 3,
    Done = 5
};
```

---

#### ⚙️ Scoped Enum (`enum class`) – C++11

```cpp
enum class Direction {
    Left,
    Right,
    Up,
    Down
};
```

* **Strongly typed** and **scoped**
* No implicit conversion to `int`
* Must access with scope:

```cpp
Direction d = Direction::Left;
```

---

#### ✅ Advantages of `enum class` over `enum`

| Feature                    | `enum` | `enum class` |
| -------------------------- | ------ | ------------ |
| Scoped                     | ❌      | ✅            |
| Implicit conversion to int | ✅      | ❌            |
| Type safe                  | ❌      | ✅            |

---

#### ✔️ Summary

* Use `enum` for simple integer constants.
* Use `enum class` for type safety and better scoping.


