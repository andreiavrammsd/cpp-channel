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
    const auto in = [](messages& ch, std::size_t thread, std::chrono::milliseconds pause) {
        thread_local static std::size_t i = 0U;

        while (true) {
            if (ch.closed()) {
                return;
            }

            ++i;
            ch << std::string{std::to_string(i) + " from: " + std::to_string(thread)};

            std::this_thread::sleep_for(pause);
        }
    };

    std::vector<std::future<void>> in_futures;
    for (std::size_t i = 0U; i < threads; ++i) {
        in_futures.push_back(std::async(in, std::ref(channel), i, std::chrono::milliseconds{500U}));
    }

    // Stream incoming data to a destination
    const auto out = [](messages& ch, std::ostream& stream, const std::string& separator) {
        std::move(ch.begin(), ch.end(), std::ostream_iterator<std::string>(stream, separator.c_str()));
    };
    const auto out_future = std::async(out, std::ref(channel), std::ref(std::cout), "\n");

    // Close the channel after some time
    const auto timeout = [](messages& ch, std::chrono::milliseconds after) {
        std::this_thread::sleep_for(after);
        ch.close();
    };
    const auto timeout_future = std::async(timeout, std::ref(channel), std::chrono::milliseconds{3000U});

    out_future.wait();
    for (auto& future : in_futures) {
        future.wait();
    }
    timeout_future.wait();
}
