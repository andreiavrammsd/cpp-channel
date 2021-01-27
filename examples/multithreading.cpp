#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>

#include "channel.hpp"

int main()
{
    auto threads = std::thread::hardware_concurrency();

    Channel<long long int> ch{threads};

    // Read
    auto out = [](Channel<long long int>& ch, std::size_t i) {
        for (auto out : ch) {
            std::cout << out << " from thread: " << i << '\n';
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    };

    std::vector<std::thread> reads;
    for (size_t i = 0; i < threads; i++) {
        reads.emplace_back(out, std::ref(ch), i);
    }

    // Write
    auto in = [](Channel<long long int>& ch) {
        while (true) {
            static long long int i = 0;
            ++i >> ch;
        }
    };

    auto write = std::thread{in, std::ref(ch)};

    // Wait for all threads to finish
    for (size_t i = 0; i < threads; i++) {
        reads.at(i).join();
    }

    write.join();
}
