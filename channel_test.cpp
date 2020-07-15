#include "channel.hpp"

#include <atomic>
#include <thread>

#include "gtest/gtest.h"

class ChannelTest : public ::testing::Test {
};

TEST_F(ChannelTest, PushAndFetch)
{
    Channel<int> channel;

    int in = 1;
    in >> channel;
    in = 2;
    in >> channel;

    int out;
    out << channel;
    EXPECT_EQ(1, out);

    out << channel;
    EXPECT_EQ(out, 2);
}

TEST_F(ChannelTest, size)
{
    Channel<int> channel;
    EXPECT_EQ(0, channel.size());

    int in = 1;
    in >> channel;
    EXPECT_EQ(1, channel.size());

    in << channel;
    EXPECT_EQ(0, channel.size());
}

TEST_F(ChannelTest, empty)
{
    Channel<int> channel;
    EXPECT_TRUE(channel.empty());

    int in = 1;
    in >> channel;
    EXPECT_FALSE(channel.empty());

    in << channel;
    EXPECT_TRUE(channel.empty());
}

TEST_F(ChannelTest, multithread)
{
    const int numbers = 100000;
    const long long expected = 5000050000;
    const int threads_to_read_from = 100;

    Channel<int> channel{10};

    std::mutex mtx_read;
    std::condition_variable cond_read;
    bool ready_to_read{};
    std::atomic<long long> count_numbers{};
    std::atomic<long long> sum_numbers{};

    std::mutex mtx_wait;
    std::condition_variable cond_wait;
    std::atomic<int> wait_counter{};
    wait_counter = threads_to_read_from;

    for (int i = 0; i < threads_to_read_from; ++i) {
        (std::thread{[&channel, &mtx_read, &cond_read, &ready_to_read, &count_numbers, &sum_numbers, &wait_counter,
                      &cond_wait] {
            // Wait until there is data on the channel
            std::unique_lock<std::mutex> lock{mtx_read};
            cond_read.wait(lock, [&ready_to_read] { return ready_to_read; });

            // Read until all items have been read from the channel
            while (count_numbers < numbers) {
                int out{};
                out << channel;

                sum_numbers += out;
                ++count_numbers;
            }
            --wait_counter;
            cond_wait.notify_one();
        }}).detach();
    }

    // Send numbers to channel
    for (int i = 1; i <= numbers; ++i) {
        i >> channel;

        // Notify threads than then can start reading
        if (!ready_to_read) {
            ready_to_read = true;
            cond_read.notify_all();
        }
    }

    // Wait until all items have been read
    std::unique_lock<std::mutex> lock{mtx_wait};
    cond_wait.wait(lock, [&wait_counter]() { return wait_counter == 0; });

    EXPECT_EQ(sum_numbers, expected);
}

TEST_F(ChannelTest, iterator)
{
    Channel<int> channel;
    channel_iterator<int> it(channel);

    int in = 2;
    in >> channel;
    EXPECT_EQ(*it, 2);

    in = 1;
    in >> channel;
    EXPECT_EQ(*it, 1);

    EXPECT_TRUE(it != it);
}
