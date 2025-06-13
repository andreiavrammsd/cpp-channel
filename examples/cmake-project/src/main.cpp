#include <chrono>
#include <iostream>
#include <msd/channel.hpp>
#include <thread>

using chan = msd::channel<int>;

// Continuously write input data on the incoming channel
[[noreturn]] void get_incoming(chan& incoming)
{
    while (true) {
        static int inc = 0;
        incoming << ++inc;
    }
}

// Time-consuming operation for each input value
int add(int input, int value)
{
    std::this_thread::sleep_for(std::chrono::milliseconds{500});
    return input + value;
}

// Read data from the incoming channel, process it, then send it on the outgoing channel
void transform(chan& incoming, chan& outgoing)
{
    for (auto input : incoming) {
        auto result = add(input, 2);
        outgoing << result;
    }
}

// Read result of processing from the outgoing channel and save it
void write_outgoing(chan& outgoing)
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
    chan incoming{threads};

    // The final result will be sent on the outgoing channel to be saved somewhere
    chan outgoing;

    // One thread for incoming data
    auto incoming_thread = std::thread{get_incoming, std::ref(incoming)};

    // One thread for outgoing data
    auto outgoing_thread = std::thread{write_outgoing, std::ref(outgoing)};

    // Multiple threads to process incoming data and send to outgoing
    std::vector<std::thread> process_threads;
    for (std::size_t i = 0U; i < threads; ++i) {
        process_threads.emplace_back(transform, std::ref(incoming), std::ref(outgoing));
    }

    // Join all threads
    incoming_thread.join();

    outgoing_thread.join();

    for (auto& thread : process_threads) {
        thread.join();
    }
}
