#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <sstream>
#include <thread>

#include "msd/channel.hpp"

int main()
{
    msd::channel<int> input_chan{30};
    msd::channel<int> output_chan{10};

    // Send to channel
    const auto writer = [&input_chan](int begin, int end) {
        for (int i = begin; i <= end; ++i) {
            input_chan.write(i);

            std::stringstream msg;
            msg << "Sent " << i << " from " << std::this_thread::get_id() << "\n";
            std::cout << msg.str();

            std::this_thread::sleep_for(std::chrono::milliseconds(10));  // simulate work
        }
        input_chan.close();
    };

    const auto reader = [&output_chan]() {
        for (const auto out : output_chan) {  // blocking until channel is drained (closed and empty)
            std::stringstream msg;
            msg << "Received " << out << " on " << std::this_thread::get_id() << "\n";
            std::cout << msg.str();

            std::this_thread::sleep_for(std::chrono::milliseconds(200));  // simulate work
        }
    };

    const auto transformer = [&input_chan, &output_chan]() {
        std::transform(input_chan.begin(), input_chan.end(), msd::back_inserter(output_chan),
                       [](int value) { return value * 2; });
        output_chan.close();
    };

    const auto reader_1 = std::async(std::launch::async, reader);
    const auto reader_2 = std::async(std::launch::async, reader);
    const auto writer_1 = std::async(std::launch::async, writer, 1, 30);
    const auto writer_2 = std::async(std::launch::async, writer, 31, 40);
    const auto transformer_task = std::async(std::launch::async, transformer);

    reader_1.wait();
    reader_2.wait();
    writer_1.wait();
    writer_2.wait();
    transformer_task.wait();
}
