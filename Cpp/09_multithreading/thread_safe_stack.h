#pragma once

#include <condition_variable>
#include <mutex>
#include <stack>

template <typename T>
class ThreadSafeStack {
   private:
    std::stack<T> unsafe_stack;
    std::mutex mutex_stack;
    std::condition_variable cv;

   public:
    void push_to_stack(T num);
    void pop_from_stack();
    bool compare_and_pop(T& desired);
    T get_stack_top();
    int get_stack_size();
    bool stack_empty();
    void print_stack();
};

#include "thread_safe_stack.tpp"