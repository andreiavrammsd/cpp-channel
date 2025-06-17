#include <msd/channel.hpp>

#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// Spawns multiple producers that send strings into a channel, which are streamed to std::cout from a consumer.

int main()
{
    using messages = msd::channel<std::string>;

    const auto threads = std::thread::hardware_concurrency();
    messages channel{threads};

    // Continuously get some data on multiple threads and send it all to a channel
    const auto produce = [](const std::size_t thread, const std::chrono::milliseconds pause, messages& chan) {
        thread_local static std::size_t inc = 0U;

        while (!chan.closed()) {
            ++inc;
            chan << std::string{"Streaming " + std::to_string(inc) + " from thread " + std::to_string(thread)};

            std::this_thread::sleep_for(pause);
        }
    };

    std::vector<std::future<void>> producers;
    for (std::size_t i = 0U; i < threads; ++i) {
        producers.push_back(std::async(produce, i, std::chrono::milliseconds{500}, std::ref(channel)));
    }

    // Close the channel after some time
    const auto close = [](const std::chrono::milliseconds after, messages& chan) {
        std::this_thread::sleep_for(after);
        chan.close();
    };
    const auto closer = std::async(close, std::chrono::milliseconds{3000U}, std::ref(channel));

    // Stream incoming messages
    std::move(channel.begin(), channel.end(), std::ostream_iterator<std::string>(std::cout, "\n"));

    // Wait all tasks
    for (auto& producer : producers) {
        producer.wait();
    }
    closer.wait();
}
