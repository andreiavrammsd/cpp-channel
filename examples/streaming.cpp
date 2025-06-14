#include <msd/channel.hpp>

#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

int main()
{
    using messages = msd::channel<std::string>;

    const auto threads = std::thread::hardware_concurrency();
    messages channel{threads};

    // Continuously get some data on multiple threads and send it all to a channel
    const auto input = [](messages& chan, std::size_t thread, std::chrono::milliseconds pause) {
        thread_local static std::size_t inc = 0U;

        while (!chan.closed()) {
            ++inc;
            chan << std::string{"Streaming " + std::to_string(inc) + " from thread " + std::to_string(thread)};

            std::this_thread::sleep_for(pause);
        }
    };

    std::vector<std::future<void>> in_futures;
    for (std::size_t i = 0U; i < threads; ++i) {
        in_futures.push_back(std::async(input, std::ref(channel), i, std::chrono::milliseconds{500}));
    }

    // Close the channel after some time
    const auto timeout = [](messages& chan, std::chrono::milliseconds after) {
        std::this_thread::sleep_for(after);
        chan.close();
    };
    const auto timeout_future = std::async(timeout, std::ref(channel), std::chrono::milliseconds{3000U});

    // Stream incoming data to a destination
    std::move(channel.begin(), channel.end(), std::ostream_iterator<std::string>(std::cout, "\n"));

    // Wait for other threads
    for (auto& future : in_futures) {
        future.wait();
    }
    timeout_future.wait();
}
