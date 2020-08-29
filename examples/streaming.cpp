#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <utility>

#include "channel.hpp"

int main()
{
    using messages = channel<std::string>;

    auto threads = std::thread::hardware_concurrency();
    messages ch{threads};

    // In
    auto in = [](messages& ch, int thread, std::chrono::milliseconds pause) {
        while (true) {
            static int i = 0;

            ++i;
            std::string{std::to_string(i) + " from: " + std::to_string(thread)} >> ch;

            std::this_thread::sleep_for(pause);
        }
    };

    std::vector<std::future<void>> in_futures;
    for (std::size_t i = 0; i < threads; ++i) {
        in_futures.push_back(std::async(in, std::ref(ch), i + 1, std::chrono::milliseconds{1000}));
    }

    // Out
    auto out = [](messages& ch, std::ostream& stream, const std::string& separator) {
        std::move(ch.begin(), ch.end(), std::ostream_iterator<std::string>(stream, separator.c_str()));
    };

    auto out_future = std::async(out, std::ref(ch), std::ref(std::cout), "\n");

    out_future.wait();
    for (auto& future : in_futures) {
        future.wait();
    }
}
