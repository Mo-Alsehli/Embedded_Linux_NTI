#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <semaphore>
#include <string>
#include <thread>

#include "thread_safe_queue.h"
#include "thread_safe_stack.h"

// int seconds = 0;
// std::counting_semaphore signal{0};
// std::mutex my_mutex{};

// void threadFunction() {
//     // try acquire
//     /*
//     while (true) {
//         std::chrono::seconds time{1};
//         std::this_thread::sleep_for(time);
//         seconds++;

//         if (signal.try_acquire()) {
//             break;
//         }
//     }*/

//     // normal acquire

//     signal.acquire();
//     std::cout << "Doing some work 2 \n";
// }

int main() {
    // ThreadSafeStack<int> my_stack;
    // std::thread t1([&my_stack]() {
    //     for (int i = 0; i < 4; i++) {
    //         my_stack.push_to_stack(i);
    //         std::cout << "Pushed: " << i << std::endl;
    //     }
    // });

    // // my_stack.print_stack();

    // std::thread t2([&my_stack]() {
    //     for (int i = 0; i < 4; i++) {
    //         my_stack.pop_from_stack();
    //         std::cout << "Poped: " << i << std::endl;
    //     }
    // });
    // t1.join();
    // t2.join();

    ThreadSafeQueue<int> my_q;

    std::thread t1{[&]() {
        for (int i = 0; i < 10; i++) {
            my_q.enqueu(i * 10);
            std::cout << "pushed: " << i * 10 << std::endl;
            std::chrono::milliseconds time(50);
            std::this_thread::sleep_for(time);
        }
    }};

    std::thread t2{[&]() {
        for (int i = 0; i < 10; i++) {
            std::cout << "\npoped: " << my_q.dequeu() << std::endl;
        }
    }};

    t1.join();
    t2.join();

    return 0;
}