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
    const auto out = [](msd::channel<std::int64_t>& chan, const std::size_t value) {
        for (auto number : chan) {
            std::stringstream stream;
            stream << number << " from thread: " << value << '\n';
            std::cout << stream.str();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    };

    std::vector<std::thread> reads;
    for (std::size_t i = 0U; i < threads; ++i) {
        reads.emplace_back(out, std::ref(channel), i);
    }

    // Write
    const auto input = [](msd::channel<std::int64_t>& chan) {
        while (true) {
            static std::int64_t value = 0;
            chan << ++value;
        }
    };

    auto write = std::thread{input, std::ref(channel)};

    // Join all threads
    for (std::size_t i = 0U; i < threads; ++i) {
        reads.at(i).join();
    }

    write.join();
}
