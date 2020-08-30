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

    // Continuously get some data on multiple threads and send it all to a channel
    auto in = [](messages& ch, int thread, std::chrono::milliseconds pause) {
        thread_local static int i = 0;

        while (true) {
            if (ch.closed()) {
                return;
            }

            ++i;
            std::string{std::to_string(i) + " from: " + std::to_string(thread)} >> ch;

            std::this_thread::sleep_for(pause);
        }
    };

    std::vector<std::future<void>> in_futures;
    for (std::size_t i = 0; i < threads; ++i) {
        in_futures.push_back(std::async(in, std::ref(ch), i, std::chrono::milliseconds{500}));
    }

    // Stream incoming data to a destination
    auto out = [](messages& ch, std::ostream& stream, const std::string& separator) {
        std::move(ch.begin(), ch.end(), std::ostream_iterator<std::string>(stream, separator.c_str()));
    };
    auto out_future = std::async(out, std::ref(ch), std::ref(std::cout), "\n");

    // Close the channel after some time
    auto timeout = [](messages& ch, std::chrono::milliseconds after) {
        std::this_thread::sleep_for(after);
        ch.close();
    };
    auto timeout_future = std::async(timeout, std::ref(ch), std::chrono::milliseconds{3000});

    out_future.wait();
    for (auto& future : in_futures) {
        future.wait();
    }
    timeout_future.wait();
}
