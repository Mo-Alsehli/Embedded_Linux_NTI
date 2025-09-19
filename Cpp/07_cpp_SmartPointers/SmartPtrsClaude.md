# Comprehensive Guide to Smart Pointers in C++

Smart pointers are one of the most important features of modern C++. They provide automatic memory management, helping prevent memory leaks, dangling pointers, and other common memory-related bugs that plague manual memory management with raw pointers.

## What Are Smart Pointers?

Smart pointers are objects that act like traditional pointers but provide additional functionality, primarily automatic memory management. They wrap raw pointers and automatically delete the pointed-to object when it's no longer needed, following the RAII (Resource Acquisition Is Initialization) principle.

The key smart pointers in C++ (available since C++11) are:
- `std::unique_ptr`
- `std::shared_ptr`
- `std::weak_ptr`

## std::unique_ptr

`std::unique_ptr` represents exclusive ownership of a dynamically allocated object. Only one `unique_ptr` can own a particular object at a time.

### Basic Usage

```cpp
#include <memory>
#include <iostream>

class MyClass {
public:
    MyClass(int val) : value(val) {
        std::cout << "MyClass constructed with value: " << value << std::endl;
    }
    
    ~MyClass() {
        std::cout << "MyClass with value " << value << " destroyed" << std::endl;
    }
    
    void display() { std::cout << "Value: " << value << std::endl; }
    
private:
    int value;
};

int main() {
    // Creating a unique_ptr
    std::unique_ptr<MyClass> ptr = std::make_unique<MyClass>(42);
    
    // Using the pointer
    ptr->display();
    (*ptr).display();  // Alternative syntax
    
    // No need to explicitly delete - automatic cleanup when ptr goes out of scope
    return 0;
}
```

### Key Features

**Exclusive Ownership**: Only one `unique_ptr` can own an object at a time.

```cpp
std::unique_ptr<int> ptr1 = std::make_unique<int>(42);
// std::unique_ptr<int> ptr2 = ptr1;  // ERROR: Copy constructor is deleted

// Use std::move for transfer of ownership
std::unique_ptr<int> ptr2 = std::move(ptr1);
// Now ptr1 is nullptr, ptr2 owns the object
```

**Custom Deleters**: You can provide custom deletion logic.

```cpp
// Custom deleter for arrays
auto arr_deleter = [](int* p) { 
    std::cout << "Custom array delete" << std::endl;
    delete[] p; 
};

std::unique_ptr<int[], decltype(arr_deleter)> arr(new int[10], arr_deleter);

// For FILE* resources
auto file_deleter = [](FILE* f) { 
    if (f) fclose(f); 
};

std::unique_ptr<FILE, decltype(file_deleter)> file(fopen("test.txt", "w"), file_deleter);
```

### Best Practices for unique_ptr

- Always prefer `std::make_unique` over `new`
- Use `unique_ptr` as the default choice for single ownership
- Return `unique_ptr` from factory functions
- Use `unique_ptr` for class members that need dynamic allocation

## std::shared_ptr

`std::shared_ptr` allows shared ownership of an object. Multiple `shared_ptr` instances can point to the same object, and the object is destroyed when the last `shared_ptr` is destroyed or reset.

### Basic Usage

```cpp
#include <memory>
#include <iostream>

int main() {
    // Creating shared_ptrs
    std::shared_ptr<MyClass> ptr1 = std::make_shared<MyClass>(100);
    std::cout << "Reference count: " << ptr1.use_count() << std::endl;  // 1
    
    {
        std::shared_ptr<MyClass> ptr2 = ptr1;  // Copy is allowed
        std::cout << "Reference count: " << ptr1.use_count() << std::endl;  // 2
        
        ptr1->display();
        ptr2->display();  // Same object
    }  // ptr2 goes out of scope
    
    std::cout << "Reference count: " << ptr1.use_count() << std::endl;  // 1
    
    return 0;  // Object destroyed here when ptr1 goes out of scope
}
```

### Reference Counting

`shared_ptr` uses reference counting to track how many pointers share ownership:

```cpp
std::shared_ptr<int> create_shared() {
    return std::make_shared<int>(42);
}

int main() {
    std::shared_ptr<int> ptr1 = create_shared();
    std::cout << "Count: " << ptr1.use_count() << std::endl;  // 1
    
    std::shared_ptr<int> ptr2 = ptr1;
    std::cout << "Count: " << ptr1.use_count() << std::endl;  // 2
    
    ptr2.reset();  // Explicitly release ptr2's ownership
    std::cout << "Count: " << ptr1.use_count() << std::endl;  // 1
    
    return 0;
}
```

### Thread Safety

`shared_ptr` provides thread-safe reference counting, but the pointed-to object itself is not automatically thread-safe:

```cpp
#include <thread>
#include <vector>

std::shared_ptr<int> global_ptr = std::make_shared<int>(0);

void increment() {
    for (int i = 0; i < 1000; ++i) {
        (*global_ptr)++;  // This is NOT thread-safe
    }
}

int main() {
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(increment);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "Final value: " << *global_ptr << std::endl;  // Likely not 4000
    return 0;
}
```

## std::weak_ptr

`std::weak_ptr` is a smart pointer that holds a non-owning reference to an object managed by `shared_ptr`. It's designed to break circular references and provide safe access to objects that might have been deleted.

### Breaking Circular References

```cpp
#include <memory>
#include <iostream>

class Child;

class Parent {
public:
    std::shared_ptr<Child> child;
    ~Parent() { std::cout << "Parent destroyed" << std::endl; }
};

class Child {
public:
    std::weak_ptr<Parent> parent;  // Use weak_ptr to break cycle
    ~Child() { std::cout << "Child destroyed" << std::endl; }
};

int main() {
    {
        auto parent = std::make_shared<Parent>();
        auto child = std::make_shared<Child>();
        
        parent->child = child;
        child->parent = parent;  // No circular reference issue
        
        // Check if parent still exists
        if (auto p = child->parent.lock()) {
            std::cout << "Parent still exists" << std::endl;
        }
    }  // Both objects are properly destroyed
    
    return 0;
}
```

### Safe Access with weak_ptr

```cpp
std::weak_ptr<int> weak;

{
    auto shared = std::make_shared<int>(42);
    weak = shared;
    
    // Safe access
    if (auto locked = weak.lock()) {
        std::cout << "Value: " << *locked << std::endl;
    } else {
        std::cout << "Object no longer exists" << std::endl;
    }
}  // shared goes out of scope, object is destroyed

// Try to access again
if (auto locked = weak.lock()) {
    std::cout << "Value: " << *locked << std::endl;
} else {
    std::cout << "Object no longer exists" << std::endl;  // This will print
}
```

## Advanced Topics

### Custom Deleters in shared_ptr

```cpp
class Resource {
public:
    Resource(const std::string& name) : name_(name) {
        std::cout << "Resource " << name_ << " acquired" << std::endl;
    }
    
    ~Resource() {
        std::cout << "Resource " << name_ << " destroyed" << std::endl;
    }
    
private:
    std::string name_;
};

void custom_deleter(Resource* r) {
    std::cout << "Custom deleter called" << std::endl;
    delete r;
}

int main() {
    {
        std::shared_ptr<Resource> ptr(new Resource("Test"), custom_deleter);
    }  // Custom deleter is called
    
    return 0;
}
```

### Array Support

For arrays, use the array specializations:

```cpp
// For unique_ptr
std::unique_ptr<int[]> arr = std::make_unique<int[]>(10);
arr[0] = 42;

// For shared_ptr (C++17)
std::shared_ptr<int[]> shared_arr(new int[10]);
shared_arr[0] = 42;

// Better: use std::vector instead
std::vector<int> vec(10);
vec[0] = 42;
```

### Performance Considerations

**make_shared vs shared_ptr constructor**:
```cpp
// Less efficient - two allocations
std::shared_ptr<MyClass> ptr1(new MyClass(42));

// More efficient - single allocation
std::shared_ptr<MyClass> ptr2 = std::make_shared<MyClass>(42);
```

`make_shared` allocates the object and control block together, improving cache locality and reducing allocations.

### Converting Between Smart Pointers

```cpp
// unique_ptr to shared_ptr
std::unique_ptr<int> unique = std::make_unique<int>(42);
std::shared_ptr<int> shared = std::move(unique);  // unique is now null

// shared_ptr to weak_ptr
std::weak_ptr<int> weak = shared;

// You cannot directly convert shared_ptr back to unique_ptr
// because shared ownership cannot become exclusive ownership safely
```

## Common Pitfalls and Best Practices

### Avoid Raw Pointers with Smart Pointers

```cpp
// DON'T DO THIS
int* raw = new int(42);
std::shared_ptr<int> ptr1(raw);
std::shared_ptr<int> ptr2(raw);  // Double deletion when both destruct!

// DO THIS
auto ptr1 = std::make_shared<int>(42);
auto ptr2 = ptr1;  // Proper shared ownership
```

### Don't Mix Raw Pointers and Smart Pointers

```cpp
// BAD
auto smart_ptr = std::make_shared<int>(42);
int* raw_ptr = smart_ptr.get();
// Don't store raw_ptr for later use - it might become invalid
```

### Use make_unique and make_shared

```cpp
// Preferred
auto ptr1 = std::make_unique<MyClass>(args);
auto ptr2 = std::make_shared<MyClass>(args);

// Avoid
std::unique_ptr<MyClass> ptr3(new MyClass(args));
std::shared_ptr<MyClass> ptr4(new MyClass(args));
```

### Function Parameters

```cpp
// Pass by reference when you don't need to affect ownership
void process(const MyClass& obj);

// Pass smart pointer by value when transferring ownership
void take_ownership(std::unique_ptr<MyClass> ptr);

// Pass smart pointer by reference when you might reset it
void might_reset(std::unique_ptr<MyClass>& ptr);

// For shared_ptr, usually pass by const reference
void process_shared(const std::shared_ptr<MyClass>& ptr);
```

## Choosing the Right Smart Pointer

**Use `std::unique_ptr` when**:
- You need exclusive ownership
- You want the lowest overhead
- You're replacing `new`/`delete` pairs
- You want to transfer ownership occasionally

**Use `std::shared_ptr` when**:
- Multiple objects need to share ownership
- The lifetime of the object is complex
- You're implementing observer patterns
- You need reference counting

**Use `std::weak_ptr` when**:
- You need to break circular references with `shared_ptr`
- You want to observe an object without extending its lifetime
- You need safe access to an object that might be deleted

Smart pointers are essential for modern C++ programming. They eliminate most memory management bugs while providing clear ownership semantics. Start with `unique_ptr` as your default choice, and use `shared_ptr` only when you genuinely need shared ownership. Always prefer the `make_*` functions for creating smart pointers, and remember that smart pointers are designed to make your code safer and more expressive, not necessarily faster than carefully written manual memory management.