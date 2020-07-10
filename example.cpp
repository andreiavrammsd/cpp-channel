#include <iostream>
#include <thread>

#include "channel.hpp"

int main()
{
    auto threads = std::thread::hardware_concurrency() / 2;

    Channel<long long int> ch{threads};

    // Read
    auto out = [](Channel<long long int>& ch, size_t i) {
        while (true) {
            long long int out{};
            out << ch;

            std::cout << std::to_string(i) + "." + std::to_string(out) << '\n';

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    };

    for (size_t i = 0; i < threads; i++) {
        (std::thread{out, std::ref(ch), i}).detach();
    }

    (std::thread{[](Channel<long long int>& ch) {
                     for (const auto& i : ch) {
                         std::cout << "range: " + std::to_string(i) << '\n';
                         std::this_thread::sleep_for(
                             std::chrono::milliseconds(1000));
                     }
                 },
                 std::ref(ch)})
        .detach();

    // Write
    auto in = [](Channel<long long int>& ch) {
        while (true) {
            static long long int i = 0;
            ++i >> ch;
        }
    };

    (std::thread{in, std::ref(ch)}).join();
}
