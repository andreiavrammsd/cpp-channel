#include <msd/channel.hpp>

#include <future>
#include <iostream>
#include <thread>

int main()
{
    msd::channel<int> channel{};

    // Write data on the channel until it's closed
    const auto input = [](msd::channel<int>& chan, int time_ms) {
        static int inc = 0;

        while (true) {
            if (chan.closed()) {
                break;
            }

            chan << ++inc;
            std::cout << "in: " << inc << "\n";

            std::this_thread::sleep_for(std::chrono::milliseconds{time_ms});
        }

        std::cout << "exit input\n";
    };
    const auto input_future = std::async(input, std::ref(channel), 10);

    // Close the channel after some time
    const auto timeout = [](msd::channel<int>& chan, int time_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds{time_ms});
        chan.close();
        std::cout << "exit timeout\n";
    };
    auto timeout_future = std::async(timeout, std::ref(channel), 100);

    // Display all the data from the channel
    // When the channel is closed and empty, the iteration will end
    const auto write = [](msd::channel<int>& chan, int time_ms) {
        for (auto out : chan) {
            std::cout << "out: " << out << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds{time_ms});
        }

        std::cout << "exit write\n";
    };
    const auto write_future1 = std::async(write, std::ref(channel), 1);
    const auto write_future2 = std::async(write, std::ref(channel), 100);

    input_future.wait();
    timeout_future.wait();
    write_future1.wait();
    write_future2.wait();
}
