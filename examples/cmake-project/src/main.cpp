#include <chrono>
#include <iostream>
#include <thread>

#include "channel.hpp"

// Continuously write input data on the incoming channel
[[noreturn]] void GetIncoming(channel<int>& incoming)
{
    while (true) {
        static int i = 0;
        ++i >> incoming;
    }
}

// Time-consuming operation for each input value
int Add(int in, int value)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return in + value;
}

// Read data from the incoming channel, process it, then send it on the outgoing channel
void Transform(channel<int>& incoming, channel<int>& outgoing)
{
    for (auto in : incoming) {
        auto result = Add(in, 2);
        result >> outgoing;
    }
}

// Read result of processing from the outgoing channel and save it
void WriteOutgoing(channel<int>& outgoing)
{
    for (auto out : outgoing) {
        std::cout << out << '\n';
    }
}

int main()
{
    // Number of threads to process incoming data on
    auto threads = std::thread::hardware_concurrency();

    // Data from outside the app is received on the incoming channel
    channel<int> incoming{threads};

    // The final result will be sent on the outgoing channel to be saved somewhere
    channel<int> outgoing;

    // One thread for incoming data
    auto incoming_thread = std::thread{GetIncoming, std::ref(incoming)};

    // One thread for outgoing data
    auto outgoing_thread = std::thread{WriteOutgoing, std::ref(outgoing)};

    // Multiple threads to process incoming data and send to outgoing
    std::vector<std::thread> process_threads;
    for (size_t i = 0; i < threads; i++) {
        process_threads.emplace_back(Transform, std::ref(incoming), std::ref(outgoing));
    }

    // Join all threads
    incoming_thread.join();

    outgoing_thread.join();

    for (auto& th : process_threads) {
        th.join();
    }
}
