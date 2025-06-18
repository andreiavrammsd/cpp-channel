#include <msd/channel.hpp>

#include <atomic>
#include <chrono>
#include <csignal>
#include <future>
#include <iostream>
#include <sstream>
#include <thread>

static std::atomic<bool> shutdown{false};

void handle_sigint(int)
{
    std::cout << "Waiting for channel to drain...\n";
    shutdown.store(true, std::memory_order_seq_cst);
}

// Graceful shutdown using a bounded thread-safe channel. It runs a producer that sends integers and a consumer that
// processes them. On Ctrl+C, it stops producing, closes the channel, and waits for the consumer to drain remaining
// messages before exiting.

int main()
{
    std::signal(SIGINT, handle_sigint);

    msd::channel<int> channel{10};

    // Continuously read from channel until it's drained (closed and empty)
    const auto consume = [&channel]() {
        for (const int message : channel) {
            std::stringstream stream;
            stream << message << " (" << channel.size() << ")\n";

            std::cout << stream.str();

            std::this_thread::sleep_for(std::chrono::milliseconds{100});
        }
    };

    const auto consumer = std::async(consume);

    // Continuously write to channel until process shutdown is requested
    const auto produce = [&channel]() {
        thread_local static int inc = 0;

        while (!shutdown.load(std::memory_order_seq_cst)) {
            ++inc;
            channel << inc;
        }

        channel.close();
    };

    const auto producer = std::async(produce);

    // Wait
    consumer.wait();
    producer.wait();
}
