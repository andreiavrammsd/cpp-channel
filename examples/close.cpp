#include <future>
#include <iostream>

#include "msd/channel.hpp"

int main()
{
    msd::channel<int> ch{};

    // Write data on the channel until it's closed
    auto input = [](msd::channel<int>& ch, int ms) {
        static int i = 0;

        while (true) {
            if (ch.closed()) {
                break;
            }

            ++i >> ch;
            std::cout << "in: " << i << "\n";

            std::this_thread::sleep_for(std::chrono::milliseconds{ms});
        }

        std::cout << "exit input\n";
    };
    auto input_future = std::async(input, std::ref(ch), 10);

    // Close the channel after some time
    auto timeout = [](msd::channel<int>& ch, int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds{ms});
        ch.close();
        std::cout << "exit timeout\n";
    };
    auto timeout_future = std::async(timeout, std::ref(ch), 100);

    // Display all the data from the channel
    // When the channel is closed and empty, the iteration will end
    auto write = [](msd::channel<int>& ch, int ms) {
        for (auto out : ch) {
            std::cout << "out: " << out << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds{ms});
        }

        std::cout << "exit write\n";
    };
    auto write_future = std::async(write, std::ref(ch), 1);

    input_future.wait();
    timeout_future.wait();
    write_future.wait();
}
