#include <msd/channel.hpp>

#include <future>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

int main()
{
    msd::channel<int> channel{};

    // Write data on the channel until it's closed
    const auto input = [](msd::channel<int>& chan, int time_ms) {
        static int inc = 0;

        while (!chan.closed()) {
            chan << ++inc;

            std::this_thread::sleep_for(std::chrono::milliseconds{time_ms});
        }
    };
    const auto input_future = std::async(input, std::ref(channel), 10);

    // Close the channel after some time
    const auto timeout = [](msd::channel<int>& chan, int time_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds{time_ms});

        chan.close();
    };
    auto timeout_future = std::async(timeout, std::ref(channel), 100);

    // Display all the data from the channel
    // When the channel is closed and empty, the iteration will end
    std::mutex cout_mutex;

    const auto write = [&cout_mutex](msd::channel<int>& chan, int time_ms) {
        for (auto out : chan) {
            std::string msg{"out: " + std::to_string(out) + "\n"};

            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << msg;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds{time_ms});
        }
    };
    const auto write_future1 = std::async(write, std::ref(channel), 1);
    const auto write_future2 = std::async(write, std::ref(channel), 100);

    input_future.wait();
    timeout_future.wait();
    write_future1.wait();
    write_future2.wait();
}
