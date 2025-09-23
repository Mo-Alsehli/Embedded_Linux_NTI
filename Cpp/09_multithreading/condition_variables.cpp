#include <condition_variable>
#include <iostream>
#include <mutex>
#include <semaphore>
#include <thread>

// std::binary_semaphore sem(0);
std::condition_variable cv;
std::mutex my_mutex;

void thread_fun() {
    std::unique_lock l{my_mutex};
    std::cout << "Before Notification\n";
    cv.wait(l);
    std::cout << "After Notification\n";
}

int main() {
    std::thread t1{thread_fun};
    std::cout << "press any key to send notification..\n";
    std::cin.get();
    cv.notify_one();

    t1.join();
}