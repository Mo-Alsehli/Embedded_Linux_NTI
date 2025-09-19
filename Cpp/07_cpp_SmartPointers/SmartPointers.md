# Smart Pointers CPP

* **Unique Pointers are located at `memroy` library**


## Unique Pointer
- It's the only owenr for a specific resource.
- syntax.
```cpp
std::unique_ptr<int> p{new int{5}};
std::unique_ptr<int> p2 = std::make_unique<int>(5);
```
- changing the ownership
```cpp
p2 = std::move(p);
// now `p` is valid pointer but it's a nullptr needs to be reallocated.
```
- getting the address or the raw owner pointer
```cpp
p2.get();
```


## Shared Pointer
- It a type of smart pointers where multiple shared pointers can own the same resource.
- The owned resource wouldn't be released unless all shared pointers to it are released.
- syntax
```cpp
std::shared_ptr<T> p = std::make_shared<T>(value);
```

- Sharing the ownership
```cpp
std::sharedptr<T> p2 = p; // Assignment operator is valid here not like unique_ptr.
```
- Shared pointers have the problem of **Circular reference** *where a shared pointers object has a member to another shared pointer to another object and that other object has a shared ptr to the first one*. 


## Weak Pointer
-  holds a non-owning reference to an object managed by shared_ptr. It's designed to break circular references and provide safe access to objects that might have been deleted.

