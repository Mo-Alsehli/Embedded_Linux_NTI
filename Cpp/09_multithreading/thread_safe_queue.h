#pragma once

#include <condition_variable>
#include <deque>
#include <iostream>
#include <mutex>

template <typename T>
class ThreadSafeQueue {
   private:
    std::mutex q_mutex;
    std::condition_variable cv;
    std::deque<T> queue;
    size_t len = 1;

   public:
    void enqueu(T element) {
        std::lock_guard lock(q_mutex);
        queue.push_back(element);
        len++;
        cv.notify_all();
    }

    T dequeu() {
        std::unique_lock l(q_mutex);
        cv.wait(l, [&]() { return !(this->empty()); });
        T front = queue[0];
        queue.pop_front();
        len--;
        return front;
    }

    int empty() { return len == 0; }
    int size() { return len; }

    T front() { return queue[0]; }

    T rear() { return queue[len - 1]; }

    void print() {
        std::unique_lock l(q_mutex);
        for (auto el : queue) {
            std::cout << el << " ";
        }
        std::cout << std::endl;
    }
};