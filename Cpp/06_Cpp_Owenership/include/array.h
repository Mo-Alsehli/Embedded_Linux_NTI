/*
// Class Array
// Constructor takes the size
// has a function read(int index) -> value
// function write(int index, int value).
*/
#include <iostream>

// ---- Define ArrayWrapper FIRST (must be a complete type for Array's member) ----
template <typename T>
class ArrayWrapper {
   public:
    // non-copyable (move-only)
    ArrayWrapper(const ArrayWrapper &) = delete;
    ArrayWrapper &operator=(const ArrayWrapper &) = delete;

    ArrayWrapper(int n) {
        std::cout << "Array Wrapper Called" << std::endl;
        size = n;
        m_arr = new T[n];
    }

    // move constructor
    ArrayWrapper(ArrayWrapper &&other) noexcept {
        size = other.size;
        m_arr = other.m_arr;
        other.size = 0;
        other.m_arr = nullptr;
    }

    // move assignment
    ArrayWrapper &operator=(ArrayWrapper &&other) noexcept {
        if (this != &other) {
            delete[] m_arr;
            size = other.size;
            m_arr = other.m_arr;
            other.size = 0;
            other.m_arr = nullptr;
        }
        return *this;
    }

    ~ArrayWrapper() {
        std::cout << "Array Wrapper Called" << std::endl;
        delete[] m_arr;
    }

    T &operator[](int i) { return m_arr[i]; }
    const T &operator[](int i) const { return m_arr[i]; }  // needed for const access

    T &operator*() { return *m_arr; }

   private:
    T *m_arr;
    int size;
};

// ---- Array comes AFTER the wrapper ----
template <typename T>
class Array {
    ArrayWrapper<T> arr;
    int curr_size;

   public:
    // Disable default constructor
    Array() = delete;
    Array(int size);
    ~Array();
    // Disable default copy constructor.
    Array(const Array &source) = delete;

    Array &operator=(const Array &s);
    T &operator[](int index);

    void write(int index, T value);
    T read(int index);
    int get_arr_size();
};
