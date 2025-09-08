#include "array.h"

#include <utility>  // for std::move

template <typename T>
Array<T>::Array(int size) : arr(size), curr_size(size) {}

template <typename T>
Array<T>::~Array() {}

template <typename T>
Array<T> &Array<T>::operator=(const Array<T> &s) {
    // Ensure not assigning to myself.
    if (&s == this) return *this;

    // Allocate a temporary wrapper with the right size
    ArrayWrapper<T> tmp(s.curr_size);

    // Deep copy elements
    for (int i = 0; i < s.curr_size; i++) {
        tmp[i] = s.arr[i];
    }

    // Move-assign the wrapper and size
    arr = std::move(tmp);
    curr_size = s.curr_size;

    return *this;
}

// [] Operator Overloading
template <typename T>
T &Array<T>::operator[](int index) {
    if (index < 0 || index >= curr_size) {
        std::cout << "ERROR: out of bounds access\n";
        return arr[0];
    }

    return arr[index];
}

template <typename T>
void Array<T>::write(int index, T value) {
    if (index < 0 || index >= curr_size) {
        std::cout << "ERROR: Index out of size !!!" << std::endl;
        return;
    }
    arr[index] = value;
    std::cout << value << " inserted at " << index << " successfully :)" << std::endl;
}

template <typename T>
T Array<T>::read(int index) {
    if (index < 0 || index >= curr_size) {
        std::cout << "Invalid Index !!!" << std::endl;
        return -1;
    }
    return arr[index];
}

template <typename T>
int Array<T>::get_arr_size() {
    return curr_size;
}

// Explicit instantiation for the type you use in main.cpp
template class Array<int>;
