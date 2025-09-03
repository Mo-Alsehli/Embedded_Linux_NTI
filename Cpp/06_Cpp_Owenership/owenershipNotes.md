# Comprehensive Guide to Ownership in C++

## Table of Contents
1. [What is Ownership?](#what-is-ownership)
2. [Stack vs Heap Memory](#stack-vs-heap-memory)
3. [Manual Memory Management (C-style)](#manual-memory-management)
4. [RAII (Resource Acquisition Is Initialization)](#raii)
5. [Smart Pointers](#smart-pointers)
6. [Move Semantics](#move-semantics)
7. [Best Practices](#best-practices)
8. [Common Pitfalls](#common-pitfalls)

## What is Ownership?

Ownership in C++ refers to the responsibility for managing the lifetime of resources (primarily memory, but also files, network connections, etc.). The owner of a resource is responsible for:

- Allocating the resource
- Ensuring proper cleanup/deallocation
- Controlling access to the resource

C++ provides multiple ownership models, each with different trade-offs.

## Stack vs Heap Memory

### Stack Memory (Automatic Storage)

```cpp
#include <iostream>
#include <string>

void stackExample() {
    int x = 42;           // Automatic storage
    std::string name = "Alice";  // Object on stack
    
    // x and name are automatically destroyed when function ends
    // No manual cleanup needed
}

class Resource {
public:
    Resource(const std::string& name) : name_(name) {
        std::cout << "Resource " << name_ << " created\n";
    }
    
    ~Resource() {
        std::cout << "Resource " << name_ << " destroyed\n";
    }
    
private:
    std::string name_;
};

void automaticCleanup() {
    Resource r1("Stack Resource");
    // r1 automatically destroyed at end of scope
}
```

### Heap Memory (Dynamic Storage)

```cpp
#include <iostream>

void heapExample() {
    // Manual allocation - WE own this memory
    int* ptr = new int(42);
    
    std::cout << "Value: " << *ptr << std::endl;
    
    // MUST manually deallocate - our responsibility!
    delete ptr;
    ptr = nullptr;  // Good practice to avoid dangling pointers
}

void problematicExample() {
    int* ptr = new int(42);
    
    if (someCondition()) {
        return;  // MEMORY LEAK! We forgot to delete
    }
    
    delete ptr;  // This might not be reached
}
```

## Manual Memory Management

### Basic New/Delete

```cpp
#include <iostream>

class Person {
public:
    Person(const std::string& name, int age) : name_(name), age_(age) {
        std::cout << "Person " << name_ << " created\n";
    }
    
    ~Person() {
        std::cout << "Person " << name_ << " destroyed\n";
    }
    
    void introduce() const {
        std::cout << "Hi, I'm " << name_ << ", age " << age_ << std::endl;
    }
    
private:
    std::string name_;
    int age_;
};

void manualMemoryExample() {
    // Single object
    Person* person = new Person("Alice", 25);
    person->introduce();
    delete person;
    
    // Array of objects
    Person* people = new Person[3]{
        Person("Bob", 30),
        Person("Carol", 28),
        Person("Dave", 35)
    };
    
    for (int i = 0; i < 3; ++i) {
        people[i].introduce();
    }
    
    delete[] people;  // Note: delete[] for arrays!
}
```

### Problems with Manual Management

```cpp
#include <iostream>
#include <stdexcept>

class ProblematicClass {
public:
    ProblematicClass() {
        data_ = new int[1000];
        other_data_ = new double[500];
        
        // If this throws, we have a memory leak!
        if (rand() % 2) {
            throw std::runtime_error("Random failure");
        }
    }
    
    ~ProblematicClass() {
        delete[] data_;
        delete[] other_data_;
    }
    
private:
    int* data_;
    double* other_data_;
};

void demonstrateProblems() {
    try {
        ProblematicClass obj;  // Might leak memory if constructor throws
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
}
```

## RAII (Resource Acquisition Is Initialization)

RAII is a C++ programming technique where resource acquisition and release are tied to object lifetime.

### Basic RAII Example

```cpp
#include <iostream>
#include <fstream>

class FileManager {
public:
    FileManager(const std::string& filename) {
        file_.open(filename);
        if (!file_.is_open()) {
            throw std::runtime_error("Failed to open file");
        }
        std::cout << "File opened: " << filename << std::endl;
    }
    
    ~FileManager() {
        if (file_.is_open()) {
            file_.close();
            std::cout << "File closed automatically\n";
        }
    }
    
    void writeData(const std::string& data) {
        file_ << data << std::endl;
    }
    
private:
    std::ofstream file_;
};

void raiExample() {
    try {
        FileManager fm("example.txt");
        fm.writeData("Hello, RAII!");
        // File automatically closed when fm goes out of scope
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        // File still properly closed due to RAII
    }
}
```

### Custom RAII Wrapper

```cpp
#include <iostream>

template<typename T>
class RAIIWrapper {
public:
    explicit RAIIWrapper(T* ptr) : ptr_(ptr) {
        std::cout << "RAIIWrapper taking ownership\n";
    }
    
    ~RAIIWrapper() {
        delete ptr_;
        std::cout << "RAIIWrapper cleaning up\n";
    }
    
    // Disable copy to prevent double deletion
    RAIIWrapper(const RAIIWrapper&) = delete;
    RAIIWrapper& operator=(const RAIIWrapper&) = delete;
    
    // Access operators
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
    T* get() const { return ptr_; }
    
private:
    T* ptr_;
};

void customRAIIExample() {
    {
        RAIIWrapper<Person> person(new Person("RAII Person", 30));
        person->introduce();
        // Automatic cleanup when person goes out of scope
    }
    std::cout << "Person has been cleaned up\n";
}
```

## Smart Pointers

Modern C++ provides smart pointers that implement RAII automatically.

### std::unique_ptr - Exclusive Ownership

```cpp
#include <memory>
#include <iostream>
#include <vector>

void uniquePtrBasics() {
    // Creating unique_ptr
    std::unique_ptr<Person> person1 = std::make_unique<Person>("Alice", 25);
    
    // Alternative creation (less preferred)
    std::unique_ptr<Person> person2(new Person("Bob", 30));
    
    // Use like regular pointer
    person1->introduce();
    (*person1).introduce();
    
    // Automatic cleanup - no delete needed!
}

void uniquePtrTransfer() {
    std::unique_ptr<Person> person1 = std::make_unique<Person>("Charlie", 35);
    
    // Transfer ownership (move)
    std::unique_ptr<Person> person2 = std::move(person1);
    
    // person1 is now nullptr, person2 owns the object
    if (!person1) {
        std::cout << "person1 is empty\n";
    }
    
    person2->introduce();
}

class ResourceManager {
public:
    void addPerson(std::unique_ptr<Person> person) {
        people_.push_back(std::move(person));
    }
    
    void listPeople() const {
        for (const auto& person : people_) {
            person->introduce();
        }
    }
    
private:
    std::vector<std::unique_ptr<Person>> people_;
};

void uniquePtrInContainers() {
    ResourceManager manager;
    
    manager.addPerson(std::make_unique<Person>("Alice", 25));
    manager.addPerson(std::make_unique<Person>("Bob", 30));
    
    manager.listPeople();
    // All people automatically cleaned up when manager is destroyed
}
```

### std::shared_ptr - Shared Ownership

```cpp
#include <memory>
#include <iostream>
#include <vector>

void sharedPtrBasics() {
    // Creating shared_ptr
    std::shared_ptr<Person> person1 = std::make_shared<Person>("Alice", 25);
    
    std::cout << "Reference count: " << person1.use_count() << std::endl;  // 1
    
    {
        std::shared_ptr<Person> person2 = person1;  // Copy - increases ref count
        std::cout << "Reference count: " << person1.use_count() << std::endl;  // 2
        
        person2->introduce();
    }  // person2 destroyed, ref count decreases
    
    std::cout << "Reference count: " << person1.use_count() << std::endl;  // 1
    
    // Object destroyed when last shared_ptr is destroyed
}

class Team {
public:
    void addMember(std::shared_ptr<Person> person) {
        members_.push_back(person);
    }
    
    void listMembers() const {
        for (const auto& member : members_) {
            member->introduce();
            std::cout << "  Ref count: " << member.use_count() << std::endl;
        }
    }
    
private:
    std::vector<std::shared_ptr<Person>> members_;
};

void sharedOwnershipExample() {
    auto alice = std::make_shared<Person>("Alice", 25);
    
    Team team1, team2;
    
    // Alice can be in multiple teams
    team1.addMember(alice);
    team2.addMember(alice);
    
    std::cout << "Alice ref count: " << alice.use_count() << std::endl;  // 3
    
    team1.listMembers();
    // Alice is destroyed only when all references are gone
}
```

### std::weak_ptr - Non-owning Reference

```cpp
#include <memory>
#include <iostream>

class Parent;
class Child;

class Parent {
public:
    Parent(const std::string& name) : name_(name) {
        std::cout << "Parent " << name_ << " created\n";
    }
    
    ~Parent() {
        std::cout << "Parent " << name_ << " destroyed\n";
    }
    
    void addChild(std::shared_ptr<Child> child) {
        children_.push_back(child);
    }
    
    void listChildren() const;
    
private:
    std::string name_;
    std::vector<std::shared_ptr<Child>> children_;
};

class Child {
public:
    Child(const std::string& name) : name_(name) {
        std::cout << "Child " << name_ << " created\n";
    }
    
    ~Child() {
        std::cout << "Child " << name_ << " destroyed\n";
    }
    
    void setParent(std::shared_ptr<Parent> parent) {
        parent_ = parent;  // weak_ptr doesn't increase ref count
    }
    
    void visitParent() const {
        if (auto parent = parent_.lock()) {  // Convert weak_ptr to shared_ptr
            std::cout << name_ << " visits parent\n";
        } else {
            std::cout << name_ << "'s parent is no longer available\n";
        }
    }
    
    const std::string& getName() const { return name_; }
    
private:
    std::string name_;
    std::weak_ptr<Parent> parent_;  // Doesn't create circular reference
};

void Parent::listChildren() const {
    for (const auto& child : children_) {
        std::cout << "  Child: " << child->getName() << std::endl;
    }
}

void weakPtrExample() {
    auto parent = std::make_shared<Parent>("John");
    auto child1 = std::make_shared<Child>("Alice");
    auto child2 = std::make_shared<Child>("Bob");
    
    // Set up relationships
    parent->addChild(child1);
    parent->addChild(child2);
    child1->setParent(parent);
    child2->setParent(parent);
    
    child1->visitParent();
    
    // When parent goes out of scope, children can detect it
    parent.reset();  // Explicitly destroy parent
    
    child1->visitParent();  // Parent no longer available
}
```

## Move Semantics

Move semantics allow transfer of ownership without copying, improving performance.

### Basic Move Example

```cpp
#include <iostream>
#include <vector>
#include <string>

class LargeResource {
public:
    LargeResource(size_t size) : size_(size) {
        data_ = new int[size_];
        std::cout << "LargeResource created (size: " << size_ << ")\n";
    }
    
    // Copy constructor (expensive)
    LargeResource(const LargeResource& other) : size_(other.size_) {
        data_ = new int[size_];
        std::copy(other.data_, other.data_ + size_, data_);
        std::cout << "LargeResource copied (expensive!)\n";
    }
    
    // Move constructor (cheap)
    LargeResource(LargeResource&& other) noexcept 
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
        std::cout << "LargeResource moved (cheap!)\n";
    }
    
    // Copy assignment
    LargeResource& operator=(const LargeResource& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            data_ = new int[size_];
            std::copy(other.data_, other.data_ + size_, data_);
            std::cout << "LargeResource copy-assigned\n";
        }
        return *this;
    }
    
    // Move assignment
    LargeResource& operator=(LargeResource&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
            std::cout << "LargeResource move-assigned\n";
        }
        return *this;
    }
    
    ~LargeResource() {
        delete[] data_;
        if (size_ > 0) {
            std::cout << "LargeResource destroyed\n";
        }
    }
    
    size_t getSize() const { return size_; }
    
private:
    int* data_;
    size_t size_;
};

void moveExample() {
    std::vector<LargeResource> resources;
    
    // This will use move constructor
    resources.push_back(LargeResource(1000));
    
    LargeResource resource(2000);
    
    // This will copy unless we explicitly move
    resources.push_back(resource);           // Copy
    resources.push_back(std::move(resource)); // Move
    
    // resource is now in a "moved-from" state - still valid but unspecified
    std::cout << "Moved-from size: " << resource.getSize() << std::endl;
}
```

### Perfect Forwarding

```cpp
#include <iostream>
#include <memory>
#include <utility>

template<typename T, typename... Args>
std::unique_ptr<T> make_unique_custom(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class ComplexObject {
public:
    ComplexObject(const std::string& name, int value, double ratio) 
        : name_(name), value_(value), ratio_(ratio) {
        std::cout << "ComplexObject created: " << name_ << std::endl;
    }
    
    // Move constructor
    ComplexObject(ComplexObject&& other) noexcept
        : name_(std::move(other.name_)), value_(other.value_), ratio_(other.ratio_) {
        std::cout << "ComplexObject moved\n";
    }
    
private:
    std::string name_;
    int value_;
    double ratio_;
};

void perfectForwardingExample() {
    // Arguments perfectly forwarded to constructor
    auto obj1 = make_unique_custom<ComplexObject>("Object1", 42, 3.14);
    
    std::string name = "Object2";
    auto obj2 = make_unique_custom<ComplexObject>(std::move(name), 100, 2.71);
}
```

## Smart Pointers Advanced Usage

### Custom Deleters

```cpp
#include <memory>
#include <iostream>
#include <cstdio>

// Custom deleter for C resources
struct FileDeleter {
    void operator()(FILE* file) const {
        if (file) {
            std::fclose(file);
            std::cout << "File closed by custom deleter\n";
        }
    }
};

using FilePtr = std::unique_ptr<FILE, FileDeleter>;

FilePtr openFile(const char* filename, const char* mode) {
    return FilePtr(std::fopen(filename, mode));
}

// Custom deleter with state
class ArrayDeleter {
public:
    ArrayDeleter(size_t size) : size_(size) {}
    
    void operator()(int* ptr) const {
        std::cout << "Deleting array of size " << size_ << std::endl;
        delete[] ptr;
    }
    
private:
    size_t size_;
};

void customDeleterExample() {
    // File management
    {
        auto file = openFile("test.txt", "w");
        if (file) {
            std::fprintf(file.get(), "Hello, custom deleter!\n");
        }
        // File automatically closed
    }
    
    // Array with custom deleter
    {
        size_t size = 100;
        std::unique_ptr<int[], ArrayDeleter> arr(new int[size], ArrayDeleter(size));
        // Array automatically deleted with size information
    }
}
```

### Shared Pointer with Custom Allocator

```cpp
#include <memory>
#include <iostream>

class TrackedResource {
public:
    TrackedResource(int id) : id_(id) {
        std::cout << "TrackedResource " << id_ << " created\n";
    }
    
    ~TrackedResource() {
        std::cout << "TrackedResource " << id_ << " destroyed\n";
    }
    
    int getId() const { return id_; }
    
private:
    int id_;
};

void sharedPtrAdvanced() {
    // Using make_shared (preferred - single allocation)
    auto resource1 = std::make_shared<TrackedResource>(1);
    
    // Creating shared_ptr with custom deleter
    auto resource2 = std::shared_ptr<TrackedResource>(
        new TrackedResource(2),
        [](TrackedResource* ptr) {
            std::cout << "Custom deleter called for resource " << ptr->getId() << std::endl;
            delete ptr;
        }
    );
    
    // Creating weak references
    std::weak_ptr<TrackedResource> weak_ref = resource1;
    
    {
        auto shared_ref = weak_ref.lock();
        if (shared_ref) {
            std::cout << "Accessed via weak_ptr: " << shared_ref->getId() << std::endl;
        }
    }
    
    resource1.reset();  // Destroy resource1
    
    // Try to access via weak_ptr after destruction
    auto shared_ref = weak_ref.lock();
    if (!shared_ref) {
        std::cout << "Resource no longer available via weak_ptr\n";
    }
}
```

## Ownership Transfer Patterns

### Factory Functions

```cpp
#include <memory>
#include <iostream>
#include <string>

class Database {
public:
    Database(const std::string& connection) : connection_(connection) {
        std::cout << "Connected to database: " << connection_ << std::endl;
    }
    
    ~Database() {
        std::cout << "Database connection closed\n";
    }
    
    void query(const std::string& sql) {
        std::cout << "Executing: " << sql << std::endl;
    }
    
private:
    std::string connection_;
};

// Factory function - returns ownership to caller
std::unique_ptr<Database> createDatabase(const std::string& type) {
    if (type == "mysql") {
        return std::make_unique<Database>("mysql://localhost:3306");
    } else if (type == "postgres") {
        return std::make_unique<Database>("postgres://localhost:5432");
    }
    return nullptr;
}

// Function takes ownership
void processDatabase(std::unique_ptr<Database> db) {
    if (db) {
        db->query("SELECT * FROM users");
        // db is destroyed when function ends
    }
}

// Function borrows (doesn't take ownership)
void useDatabase(const Database& db) {
    // We can use db but don't own it
    const_cast<Database&>(db).query("SELECT COUNT(*) FROM products");
}

void ownershipTransferExample() {
    auto db = createDatabase("mysql");
    
    if (db) {
        useDatabase(*db);       // Borrow
        processDatabase(std::move(db));  // Transfer ownership
        
        // db is now nullptr
        if (!db) {
            std::cout << "Database ownership transferred\n";
        }
    }
}
```

### Observer Pattern with Weak References

```cpp
#include <memory>
#include <vector>
#include <iostream>
#include <algorithm>

class Subject;

class Observer {
public:
    virtual ~Observer() = default;
    virtual void notify(const std::string& message) = 0;
};

class ConcreteObserver : public Observer {
public:
    ConcreteObserver(const std::string& name) : name_(name) {}
    
    void notify(const std::string& message) override {
        std::cout << name_ << " received: " << message << std::endl;
    }
    
private:
    std::string name_;
};

class Subject {
public:
    void addObserver(std::shared_ptr<Observer> observer) {
        observers_.push_back(observer);  // Store as weak_ptr
    }
    
    void notifyObservers(const std::string& message) {
        // Clean up expired weak_ptrs and notify valid ones
        auto it = std::remove_if(observers_.begin(), observers_.end(),
            [&message](const std::weak_ptr<Observer>& weak_obs) {
                if (auto observer = weak_obs.lock()) {
                    observer->notify(message);
                    return false;  // Keep this observer
                }
                return true;  // Remove expired observer
            });
        
        observers_.erase(it, observers_.end());
        
        std::cout << "Active observers: " << observers_.size() << std::endl;
    }
    
private:
    std::vector<std::weak_ptr<Observer>> observers_;
};

void observerPatternExample() {
    Subject subject;
    
    {
        auto observer1 = std::make_shared<ConcreteObserver>("Observer1");
        auto observer2 = std::make_shared<ConcreteObserver>("Observer2");
        
        subject.addObserver(observer1);
        subject.addObserver(observer2);
        
        subject.notifyObservers("First message");
        
        // observer2 goes out of scope
    }
    
    subject.notifyObservers("Second message");  // Only observer1 receives this
}
```

## Best Practices

### 1. Prefer Automatic Storage When Possible

```cpp
#include <iostream>
#include <vector>

// Good: Use automatic storage
void processData() {
    std::vector<int> data{1, 2, 3, 4, 5};  // Automatic storage
    
    for (int value : data) {
        std::cout << value << " ";
    }
    // data automatically cleaned up
}

// Avoid: Unnecessary dynamic allocation
void avoidThis() {
    std::vector<int>* data = new std::vector<int>{1, 2, 3, 4, 5};
    
    for (int value : *data) {
        std::cout << value << " ";
    }
    
    delete data;  // Manual cleanup required
}
```

### 2. Use make_unique and make_shared

```cpp
#include <memory>

class Resource {
public:
    Resource(int value) : value_(value) {}
    int getValue() const { return value_; }
private:
    int value_;
};

void bestPractices() {
    // Preferred: Exception safe, single allocation
    auto resource1 = std::make_unique<Resource>(42);
    auto resource2 = std::make_shared<Resource>(24);
    
    // Avoid: Potential exception safety issues
    // std::unique_ptr<Resource> resource3(new Resource(13));
}
```

### 3. Clear Ownership Semantics in APIs

```cpp
#include <memory>
#include <vector>

class DataProcessor {
public:
    // Takes ownership
    void setResource(std::unique_ptr<Resource> resource) {
        resource_ = std::move(resource);
    }
    
    // Shares ownership
    void addSharedResource(std::shared_ptr<Resource> resource) {
        shared_resources_.push_back(resource);
    }
    
    // Borrows (doesn't take ownership)
    void processResource(const Resource& resource) {
        std::cout << "Processing resource with value: " << resource.getValue() << std::endl;
    }
    
    // Returns ownership
    std::unique_ptr<Resource> createResource(int value) {
        return std::make_unique<Resource>(value);
    }
    
private:
    std::unique_ptr<Resource> resource_;
    std::vector<std::shared_ptr<Resource>> shared_resources_;
};
```

## Common Pitfalls

### 1. Dangling Pointers

```cpp
#include <iostream>

void danglingPointerProblem() {
    int* ptr;
    
    {
        int x = 42;
        ptr = &x;  // ptr points to local variable
    }  // x is destroyed here
    
    // std::cout << *ptr << std::endl;  // UNDEFINED BEHAVIOR!
}

void solutionWithSmartPointer() {
    std::unique_ptr<int> ptr;
    
    {
        auto local_ptr = std::make_unique<int>(42);
        ptr = std::move(local_ptr);  // Transfer ownership
    }  // local_ptr is empty, but object still alive
    
    std::cout << *ptr << std::endl;  // Safe!
}
```

### 2. Circular References

```cpp
#include <memory>
#include <iostream>

// PROBLEM: Circular reference with shared_ptr
class BadParent;
class BadChild;

class BadParent {
public:
    ~BadParent() { std::cout << "BadParent destroyed\n"; }
    std::shared_ptr<BadChild> child;
};

class BadChild {
public:
    ~BadChild() { std::cout << "BadChild destroyed\n"; }
    std::shared_ptr<BadParent> parent;  // Creates circular reference!
};

void circularReferenceProblem() {
    auto parent = std::make_shared<BadParent>();
    auto child = std::make_shared<BadChild>();
    
    parent->child = child;
    child->parent = parent;  // Circular reference - memory leak!
    
    // Neither destructor will be called!
}

// SOLUTION: Use weak_ptr to break cycles
class GoodParent {
public:
    ~GoodParent() { std::cout << "GoodParent destroyed\n"; }
    std::shared_ptr<BadChild> child;
};

class GoodChild {
public:
    ~GoodChild() { std::cout << "GoodChild destroyed\n"; }
    std::weak_ptr<GoodParent> parent;  // weak_ptr breaks the cycle
};

void circularReferenceSolution() {
    auto parent = std::make_shared<GoodParent>();
    auto child = std::make_shared<BadChild>();
    
    parent->child = child;
    child->parent = parent;  // No circular reference with weak_ptr
    
    // Both destructors will be called properly
}
```

### 3. Use After Move

```cpp
#include <iostream>
#include <string>

void useAfterMoveProblem() {
    std::string str = "Hello, World!";
    std::string moved_str = std::move(str);
    
    // str is now in "moved-from" state
    // Don't use str except for assignment or destruction
    // std::cout << str << std::endl;  // AVOID: Behavior is unspecified
    
    // Safe operations on moved-from objects:
    str = "New value";  // Assignment is OK
    str.clear();        // Member functions that don't depend on state
    
    std::cout << moved_str << std::endl;  // This is fine
}
```

### 4. Ownership Confusion

```cpp
#include <memory>
#include <iostream>

class ConfusingOwnership {
public:
    // BAD: Unclear who owns the pointer
    void setData(int* data) {
        data_ = data;  // Do we own this? Do we delete it?
    }
    
    // BETTER: Clear ownership semantics
    void setDataOwning(std::unique_ptr<int> data) {
        owned_data_ = std::move(data);  // We clearly take ownership
    }
    
    void setDataBorrowing(const int& data) {
        borrowed_data_ = &data;  // We clearly don't own this
    }
    
private:
    int* data_;                           // Unclear ownership
    std::unique_ptr<int> owned_data_;     // We own this
    const int* borrowed_data_;            // We don't own this
};
```

## Complete Example: Resource Manager

```cpp
#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

class ManagedResource {
public:
    ManagedResource(const std::string& name) : name_(name), id_(next_id_++) {
        std::cout << "Resource " << name_ << " (ID: " << id_ << ") created\n";
    }
    
    ~ManagedResource() {
        std::cout << "Resource " << name_ << " (ID: " << id_ << ") destroyed\n";
    }
    
    const std::string& getName() const { return name_; }
    int getId() const { return id_; }
    
private:
    std::string name_;
    int id_;
    static int next_id_;
};

int ManagedResource::next_id_ = 1;

class ResourceManager {
public:
    // Takes exclusive ownership
    void addExclusiveResource(std::unique_ptr<ManagedResource> resource) {
        std::cout << "Adding exclusive resource: " << resource->getName() << std::endl;
        exclusive_resources_.push_back(std::move(resource));
    }
    
    // Shares ownership
    void addSharedResource(std::shared_ptr<ManagedResource> resource) {
        std::cout << "Adding shared resource: " << resource->getName() 
                  << " (ref count: " << resource.use_count() << ")\n";
        shared_resources_.push_back(resource);
    }
    
    // Creates and returns ownership
    std::unique_ptr<ManagedResource> createResource(const std::string& name) {
        return std::make_unique<ManagedResource>(name);
    }
    
    // Borrows for read-only access
    void listResources() const {
        std::cout << "\n=== Exclusive Resources ===\n";
        for (const auto& resource : exclusive_resources_) {
            std::cout << "  " << resource->getName() << " (ID: " << resource->getId() << ")\n";
        }
        
        std::cout << "\n=== Shared Resources ===\n";
        for (const auto& resource : shared_resources_) {
            std::cout << "  " << resource->getName() << " (ID: " << resource->getId() 
                      << ", ref count: " << resource.use_count() << ")\n";
        }
    }
    
    // Finds and returns non-owning reference
    ManagedResource* findResource(int id) {
        // Search exclusive resources
        for (const auto& resource : exclusive_resources_) {
            if (resource->getId() == id) {
                return resource.get();
            }
        }
        
        // Search shared resources
        for (const auto& resource : shared_resources_) {
            if (resource->getId() == id) {
                return resource.get();
            }
        }
        
        return nullptr;
    }
    
    // Removes and returns ownership
    std::unique_ptr<ManagedResource> removeExclusiveResource(int id) {
        auto it = std::find_if(exclusive_resources_.begin(), exclusive_resources_.end(),
            [id](const std::unique_ptr<ManagedResource>& resource) {
                return resource->getId() == id;
            });
        
        if (it != exclusive_resources_.end()) {
            auto resource = std::move(*it);
            exclusive_resources_.erase(it);
            return resource;
        }
        
        return nullptr;
    }
    
private:
    std::vector<std::unique_ptr<ManagedResource>> exclusive_resources_;
    std::vector<std::shared_ptr<ManagedResource>> shared_resources_;
};

void completeExample() {
    ResourceManager manager;
    
    // Create and add exclusive resources
    manager.addExclusiveResource(manager.createResource("Database"));
    manager.addExclusiveResource(std::make_unique<ManagedResource>("FileSystem"));
    
    // Create shared resources
    auto sharedCache = std::make_shared<ManagedResource>("Cache");
    auto sharedLogger = std::make_shared<ManagedResource>("Logger");
    
    manager.addSharedResource(sharedCache);
    manager.addSharedResource(sharedLogger);
    
    // Create additional references to shared resources
    auto anotherCacheRef = sharedCache;
    auto anotherLoggerRef = sharedLogger;
    
    manager.listResources();
    
    // Borrow a resource for temporary use
    if (auto* resource = manager.findResource(1)) {
        std::cout << "\nFound resource: " << resource->getName() << std::endl;
    }
    
    // Remove and transfer ownership
    auto removed = manager.removeExclusiveResource(1);
    if (removed) {
        std::cout << "\nRemoved and took ownership of: " << removed->getName() << std::endl;
    }
    
    manager.listResources();
    
    std::cout << "\nShared cache ref count: " << sharedCache.use_count() << std::endl;
    
    // Cleanup happens automatically
}
```

## Summary and Guidelines

### When to Use Each Ownership Model

1. **Automatic Storage (Stack)**
   - Default choice for most objects
   - When object lifetime matches scope
   - For value types and small objects

2. **std::unique_ptr**
   - Exclusive ownership
   - Polymorphic objects
   - Factory functions
   - Replacing raw pointers with single ownership

3. **std::shared_ptr**
   - Shared ownership needed
   - Object needs to outlive multiple scopes
   - Complex ownership relationships
   - Cache-like scenarios

4. **std::weak_ptr**
   - Breaking circular references
   - Observer patterns
   - Optional references to shared objects

5. **Raw Pointers**
   - Non-owning references only
   - C API interop
   - Performance-critical code (with careful design)

### Key Principles

1. **Prefer automatic storage** over dynamic allocation
2. **Use smart pointers** instead of raw new/delete
3. **Make ownership explicit** in function signatures
4. **Follow the Rule of Zero** - let compiler generate special member functions when possible
5. **Use move semantics** to transfer ownership efficiently
6. **Avoid circular references** with weak_ptr
7. **Be consistent** with ownership patterns across your codebase

### Memory Safety Checklist

- ✅ Every `new` has a corresponding `delete`
- ✅ Use smart pointers for dynamic allocation
- ✅ Prefer `make_unique` and `make_shared`
- ✅ Use `weak_ptr` to break cycles
- ✅ Mark move constructors `noexcept`
- ✅ Don't use moved-from objects (except for assignment)
- ✅ Make ownership transfer explicit in function signatures
- ✅ Use const references for borrowing
