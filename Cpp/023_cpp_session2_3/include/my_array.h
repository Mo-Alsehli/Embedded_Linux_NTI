/*
// Class Array
// Constructor takes the size
// has a function read(int index) -> value
// function write(int index, int value).
*/
#include <iostream>

template <typename T>
class Array {
    T *arr;
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

#include <my_array.tpp>