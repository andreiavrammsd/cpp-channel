#include <msd/channel.hpp>

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <thread>

int main()
{
    const auto threads = std::thread::hardware_concurrency();

    msd::channel<std::int64_t> channel{threads};

    // Read
    const auto out = [](msd::channel<std::int64_t>& ch, std::size_t i) {
        for (auto number : ch) {
            std::stringstream stream;
            stream << number << " from thread: " << i << '\n';
            std::cout << stream.str();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    };

    std::vector<std::thread> reads;
    for (std::size_t i = 0U; i < threads; ++i) {
        reads.emplace_back(out, std::ref(channel), i);
    }

    // Write
    const auto in = [](msd::channel<std::int64_t>& ch) {
        while (true) {
            static std::int64_t i = 0;
            ch << ++i;
        }
    };

    auto write = std::thread{in, std::ref(channel)};

    // Join all threads
    for (std::size_t i = 0U; i < threads; ++i) {
        reads.at(i).join();
    }

    write.join();
}
