#include <future>
#include <iostream>
#include <sstream>
#include <thread>

#include "msd/static_channel.hpp"

int main()
{
    msd::static_channel<int, 50> chan{};  // always buffered

    // Send to channel
    const auto writer = [&chan](int begin, int end) {
        for (int i = begin; i <= end; ++i) {
            chan.write(i);

            std::stringstream msg;
            msg << "Sent " << i << " from " << std::this_thread::get_id() << "\n";
            std::cout << msg.str();

            std::this_thread::sleep_for(std::chrono::milliseconds(10));  // simulate work
        }
        chan.close();
    };

    const auto reader = [&chan]() {
        for (const auto out : chan) {  // blocking until channel is drained (closed and empty)
            std::stringstream msg;
            msg << "Received " << out << " on " << std::this_thread::get_id() << "\n";
            std::cout << msg.str();

            std::this_thread::sleep_for(std::chrono::milliseconds(200));  // simulate work
        }
    };

    const auto reader_1 = std::async(std::launch::async, reader);
    const auto reader_2 = std::async(std::launch::async, reader);
    const auto reader_3 = std::async(std::launch::async, reader);
    const auto writer_1 = std::async(std::launch::async, writer, 1, 50);
    const auto writer_2 = std::async(std::launch::async, writer, 51, 100);

    reader_1.wait();
    reader_2.wait();
    reader_3.wait();
    writer_1.wait();
    writer_2.wait();
}
