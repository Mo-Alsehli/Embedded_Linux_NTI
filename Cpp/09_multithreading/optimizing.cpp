#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <semaphore>
#include <thread>

constexpr size_t SIZE = 1e9;
constexpr size_t SIZE1 = 3e8;
constexpr size_t SIZE2 = 3e8;
constexpr size_t SIZE3 = 4e8;

std::array<int, SIZE> global_arr;

void init_arr() {
    for (auto& i : global_arr) {
        i = 1;
    }
}

void sum_arr(size_t start, size_t end, int& result) {
    int sum = 0;
    for (int i = start; i < end; i++) {
        sum += global_arr[i];
    }
    result = sum;
}

int parallel_sum() {
    constexpr size_t threads = 25;
    if (SIZE % threads != 0) {
        std::cout << "[ERROR] threads must be even and (thread % 10) == 0 \n";
        return 0;
    }
    // int sum1, sum2, sum3;
    // std::thread t1{sum_arr, 0, SIZE1, std::ref(sum1)};
    // std::thread t2{sum_arr, SIZE1, SIZE1 + SIZE2, std::ref(sum2)};
    // std::thread t3{sum_arr, SIZE1 + SIZE2, SIZE, std::ref(sum3)};

    // t1.join();
    // t2.join();
    // t3.join();

    std::array<std::thread, threads> sum_threads;
    std::array<int, threads> ref_sum;
    int i = 0;
    int slice = SIZE / threads;
    int offset = 0;
    int end = slice;
    for (auto& t : sum_threads) {
        t = std::thread{sum_arr, offset, end, std::ref(ref_sum[i])};
        offset += slice;
        end += slice;
        i++;
    }

    for (auto& t : sum_threads) t.join();

    return std::accumulate(ref_sum.begin(), ref_sum.end(), 0);
}

int main() {
    init_arr();
    int result = 0;
    // std::cout << "Sequential Time: ";
    std::cout << "Threading Time: ";
    auto start = std::chrono::high_resolution_clock::now();
    // sum_arr(0, SIZE, std::ref(result));
    result = parallel_sum();
    auto end = std::chrono::high_resolution_clock::now();
    auto time = end - start;
    std::cout << (time.count() / 1e6) << "ms\n";
    std::cout << "Sum: " << result << std::endl;

    return 0;
}
