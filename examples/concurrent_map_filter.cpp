#include <msd/channel.hpp>

#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <sstream>
#include <thread>

struct message {
    int value;
};

// A concurrent data processing pipeline with multiple producers, parallel mappers, a filter stage, and a final
// consumer.

int main()
{
    // Channel to get messages from a producer
    msd::channel<message> input_chan{15};

    // Channel to write transformed messages on
    msd::channel<int> mapped_chan{10};

    // Channel to write filtered messages on
    msd::channel<int> filtered_chan{10};

    // Produce messages
    const auto produce = [&input_chan](int begin, int end) {
        for (int i = begin; i <= end; ++i) {
            input_chan << message{i};

            std::this_thread::sleep_for(std::chrono::milliseconds(10));  // simulate work
        }
    };

    // Map from message type to int
    const auto map = [&input_chan, &mapped_chan]() {
        std::transform(input_chan.begin(), input_chan.end(), msd::back_inserter(mapped_chan), [](const message& msg) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));  // simulate work

            return msg.value + 1;
        });
    };

    // Filter mapped values
    const auto filter_values = [&mapped_chan, &filtered_chan]() {
        std::copy_if(mapped_chan.begin(), mapped_chan.end(), msd::back_inserter(filtered_chan),
                     [](int value) { return value % 2 == 0; });
        filtered_chan.close();
    };

    // Consume mapped messages
    const auto consume = [&filtered_chan]() {
        int sum{};

        for (const int value : filtered_chan) {
            sum += value;

            std::stringstream msg;
            msg << "Consumer received " << value << '\n';
            std::cout << msg.str();
        }

        return sum;
    };

    // Close: this is just to end the process, but depending on your needs, you might want to run the flow forever
    const auto close = [&input_chan, &mapped_chan]() {
        std::this_thread::sleep_for(std::chrono::milliseconds{5000});
        input_chan.close();
        mapped_chan.close();
    };

    const auto mapper_1 = std::async(map);
    const auto mapper_2 = std::async(map);
    const auto mapper_3 = std::async(map);
    const auto filter = std::async(filter_values);
    const auto producer_1 = std::async(produce, 1, 30);
    const auto producer_2 = std::async(produce, 31, 40);
    const auto closer = std::async(close);

    const int result = consume();
    const int expected = 420;

    if (result != expected) {
        std::cerr << "Error: result is " << result << ", expected " << expected << '\n';
        std::terminate();
    }

    mapper_1.wait();
    mapper_2.wait();
    mapper_3.wait();
    filter.wait();
    producer_1.wait();
    producer_2.wait();
    closer.wait();
}
