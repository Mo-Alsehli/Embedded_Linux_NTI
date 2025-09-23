#pragma once

#include "thread_safe_stack.h"

template <typename T>
void ThreadSafeStack<T>::push_to_stack(T num) {
    std::lock_guard lock(mutex_stack);
    unsafe_stack.push(num);
}

template <typename T>
void ThreadSafeStack<T>::pop_from_stack() {
    std::lock_guard lock(mutex_stack);
    if (!unsafe_stack.empty()) unsafe_stack.pop();
}

template <typename T>
bool ThreadSafeStack<T>::compare_and_pop(T& desired) {
    std::lock_guard lock(mutex_stack);
    if (unsafe_stack.empty()) return false;
    if (unsafe_stack.top() == desired) {
        unsafe_stack.pop();
        return true;
    }
    return false;
}

template <typename T>
T ThreadSafeStack<T>::get_stack_top() {
    std::lock_guard lock(mutex_stack);
    return unsafe_stack.top();
}

template <typename T>
int ThreadSafeStack<T>::get_stack_size() {
    std::lock_guard lock(mutex_stack);
    return unsafe_stack.size();
}

template <typename T>
bool ThreadSafeStack<T>::stack_empty() {
    std::lock_guard lock(mutex_stack);
    return unsafe_stack.empty();
}

template <typename T>

void ThreadSafeStack<T>::print_stack() {
    std::lock_guard lock(mutex_stack);
    std::stack print_stack = unsafe_stack;
    int size = print_stack.size();
    while (size--) {
        std::cout << "stack[" << size << "]" << " = " << print_stack.top() << std::endl;
        print_stack.pop();
    }
}
