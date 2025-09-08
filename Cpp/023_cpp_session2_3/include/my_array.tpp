template <typename T>
Array<T>::Array(int size) {
    arr = new T[size];
    curr_size = size;
}

template <typename T>
Array<T>::~Array() {
    delete[] arr;
}

template <typename T>
Array<T> &Array<T>::operator=(const Array<T> &s) {
    // Insure not assigning to my slef.
    if (&s == this) return *this;

    curr_size = s.curr_size;
    delete[] arr;
    arr = new T[curr_size];
    for (int i = 0; i < curr_size; i++) {
        arr[i] = s.arr[i];
    }

    return *this;
}

// [] Operator Overloading
template <typename T>
T &Array<T>::operator[](int index) {
    if (index < 0 || index > curr_size) {
        std::cout << "ERROR: out of bounds access\n";
        return arr[0];
    }

    return arr[index];
}

template <typename T>
void Array<T>::write(int index, T value) {
    if (index > curr_size) {
        std::cout << "ERROR: Index out of size !!!" << std::endl;
        return;
    }
    arr[index] = value;
    std::cout << value << " inserted at " << index << " successfully :)" << std::endl;
}

template <typename T>
T Array<T>::read(int index) {
    if (index > curr_size) {
        std::cout << "Invalid Index !!!" << std::endl;
        return -1;
    }
    return arr[index];
}

template <typename T>
int Array<T>::get_arr_size() {
    return curr_size;
}
