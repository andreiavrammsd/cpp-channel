#include <chrono>
#include <iostream>
#include <thread>

#include "msd/channel.hpp"

// using namespace std::chrono_literals; for post-C++11 code, use this to save some headaches

void demonstrateTimeouts()
{
    // small capacity, short timeout
    msd::channel<int> ch{2};
    ch.setTimeout(std::chrono::milliseconds(100));

    std::cout << "Testing write timeout on full buffer:\n";
    try {
        ch << 1;
        ch << 2;
        std::cout << "Attempting to write to full channel...\n";
        ch << 3;
    }
    catch (const msd::channel_timeout& e) {
        std::cout << "Expected timeout occurred: " << e.what() << "\n";
    }

    std::cout << "\nTesting read timeout on empty channel:\n";

    msd::channel<int> ch2{5};
    ch2.setTimeout(std::chrono::milliseconds(200));

    try {
        int value;
        std::cout << "Attempting to read from empty channel...\n";
        ch2 >> value;
    }
    catch (const msd::channel_timeout& e) {
        std::cout << "Expected timeout occurred: " << e.what() << "\n";
    }

    std::cout << "\nDemonstrating timeout with range-based for loop:\n";

    msd::channel<int> ch3{5};
    ch3.setTimeout(std::chrono::milliseconds(200));  // lower this to see the timeout

    // Producer
    std::thread writer([&ch3]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        ch3 << 1;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        ch3 << 2;
        ch3.close();
    });

    // Consumer
    try {
        for (const auto& value : ch3) {
            std::cout << "Received value: " << value << "\n";
        }
    }
    catch (const msd::channel_timeout& e) {
        std::cout << "Timeout in for loop: " << e.what() << "\n";
    }

    writer.join();
}

int main()
{
    // small capacity, short timeout
    msd::channel<int> ch{2};
    ch.setTimeout(std::chrono::milliseconds(100));

    std::cout << "Testing write timeout on full buffer:\n";
    try {
        ch << 1;
        ch << 2;
        std::cout << "Attempting to write to full channel...\n";
        ch << 3;
    }
    catch (const msd::channel_timeout& e) {
        std::cout << "Expected timeout occurred: " << e.what() << "\n";
    }

    std::cout << "\nTesting read timeout on empty channel:\n";

    msd::channel<int> ch2{5};
    ch2.setTimeout(std::chrono::milliseconds(200));

    try {
        int value;
        std::cout << "Attempting to read from empty channel...\n";
        ch2 >> value;
    }
    catch (const msd::channel_timeout& e) {
        std::cout << "Expected timeout occurred: " << e.what() << "\n";
    }

    std::cout << "\nDemonstrating timeout with range-based for loop:\n";

    msd::channel<int> ch3{5};
    ch3.setTimeout(std::chrono::milliseconds(200));  // lower this to see the timeout

    // Producer
    std::thread writer([&ch3]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        ch3 << 1;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        ch3 << 2;
        ch3.close();
    });

    // Consumer
    try {
        for (const auto& value : ch3) {
            std::cout << "Received value: " << value << "\n";
        }
    }
    catch (const msd::channel_timeout& e) {
        std::cout << "Timeout in for loop: " << e.what() << "\n";
    }

    writer.join();

    return 0;
}
