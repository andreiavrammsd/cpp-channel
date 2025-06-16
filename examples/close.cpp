#include <msd/channel.hpp>

#include <future>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>

int main()
{
    msd::channel<int> channel{};

    // Write data on the channel until it's closed
    const auto produce = [](msd::channel<int>& chan, int time_ms) {
        static int inc = 0;

        while (!chan.closed()) {
            chan << ++inc;

            std::this_thread::sleep_for(std::chrono::milliseconds{time_ms});
        }
    };
    const auto producer = std::async(produce, std::ref(channel), 10);

    // Close the channel after some time
    const auto close = [](msd::channel<int>& chan, int time_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds{time_ms});

        chan.close();
    };
    const auto closer = std::async(close, std::ref(channel), 100);

    // Display all the data from the channel
    // When the channel is closed and empty, the iteration will end
    std::mutex mutex;

    const auto consume = [&mutex](msd::channel<int>& chan, int time_ms) {
        for (auto value : chan) {
            std::stringstream stream;
            stream << "value " << value << " from consumer " << std::this_thread::get_id() << '\n';

            {
                std::lock_guard<std::mutex> lock(mutex);

                std::cout << stream.str();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds{time_ms});
        }
    };
    const auto consumer_1 = std::async(consume, std::ref(channel), 50);
    const auto consumer_2 = std::async(consume, std::ref(channel), 10);

    producer.wait();
    closer.wait();
    consumer_1.wait();
    consumer_2.wait();
}
