#include "msd/static_channel.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

TEST(StaticChannelTest, Traits)
{
    using type = int;
    using channel = msd::static_channel<type, 1>;
    EXPECT_TRUE((std::is_same<channel::value_type, type>::value));

    using iterator = msd::blocking_iterator<msd::static_channel<type, 1>>;
    EXPECT_TRUE((std::is_same<channel::iterator, iterator>::value));

    EXPECT_TRUE((std::is_same<channel::size_type, std::size_t>::value));
}

TEST(StaticChannelTest, WriteAndRead)
{
    msd::static_channel<int, 5> channel;

    int in = 1;
    EXPECT_NO_THROW(channel << in);

    const int cin = 3;
    EXPECT_TRUE(channel.write(cin));

    channel.close();
    EXPECT_FALSE(channel.write(2));

    int out = 0;

    EXPECT_TRUE(channel.read(out));
    EXPECT_EQ(1, out);

    channel >> out;
    EXPECT_EQ(3, out);

    EXPECT_FALSE(channel.read(out));
    EXPECT_NO_THROW(channel >> out);
    EXPECT_EQ(3, out);
}

TEST(StaticChannelTest, WriteByMoveAndRead)
{
    msd::static_channel<std::string, 5> channel;

    std::string in = "abc";
    EXPECT_TRUE(channel.write(std::move(in)));

    EXPECT_TRUE(channel.write(std::string{"def"}));

    std::string out{};
    EXPECT_TRUE(channel.read(out));
    EXPECT_EQ("abc", out);

    EXPECT_TRUE(channel.read(out));
    EXPECT_EQ("def", out);
}

TEST(StaticChannelTest, size)
{
    msd::static_channel<int, 10> channel;
    EXPECT_EQ(0, channel.size());

    int in = 1;
    channel.write(in);
    EXPECT_EQ(1, channel.size());

    channel.read(in);
    EXPECT_EQ(0, channel.size());
}

TEST(StaticChannelTest, empty)
{
    msd::static_channel<int, 5> channel;
    EXPECT_TRUE(channel.empty());

    int in = 1;
    channel.write(in);
    EXPECT_FALSE(channel.empty());

    channel.read(in);
    EXPECT_TRUE(channel.empty());
}

TEST(StaticChannelTest, close)
{
    msd::static_channel<int, 5> channel;
    EXPECT_FALSE(channel.closed());

    int in = 1;
    EXPECT_TRUE(channel.write(in));

    channel.close();
    EXPECT_TRUE(channel.closed());

    int out = 0;
    EXPECT_TRUE(channel.read(out));
    EXPECT_EQ(1, out);
    EXPECT_FALSE(channel.read(out));

    EXPECT_FALSE(channel.write(in));
}

TEST(StaticChannelTest, Iterator)
{
    msd::static_channel<int, 10> channel;

    channel.write(1);

    for (auto it = channel.begin(); it != channel.end();) {
        EXPECT_EQ(1, *it);
        break;
    }
}

TEST(StaticChannelTest, Multithreading)
{
    const int numbers = 10000;
    const std::int64_t expected = 50005000;
    constexpr std::size_t kThreadsToReadFrom = 100;

    msd::static_channel<int, kThreadsToReadFrom> channel{};

    std::mutex mtx_read{};
    std::condition_variable cond_read{};
    bool ready_to_read{};
    std::atomic<std::int64_t> count_numbers{};
    std::atomic<std::int64_t> sum_numbers{};

    std::mutex mtx_wait{};
    std::condition_variable cond_wait{};
    std::atomic<std::size_t> wait_counter{kThreadsToReadFrom};

    auto worker = [&] {
        // Wait until there is data on the channel
        std::unique_lock<std::mutex> lock{mtx_read};
        cond_read.wait(lock, [&ready_to_read] { return ready_to_read; });

        // Read until all items have been read from the channel
        while (count_numbers < numbers) {
            int out{};
            channel.read(out);

            sum_numbers += out;
            ++count_numbers;
        }
        --wait_counter;
        cond_wait.notify_one();
    };

    std::vector<std::thread> threads{};
    for (std::size_t i = 0U; i < kThreadsToReadFrom; ++i) {
        threads.emplace_back(worker);
    }

    // Send numbers to channel
    for (int i = 1; i <= numbers; ++i) {
        channel.write(i);

        // Notify threads than then can start reading
        if (!ready_to_read) {
            ready_to_read = true;
            cond_read.notify_all();
        }
    }

    // Wait until all items have been read
    std::unique_lock<std::mutex> lock{mtx_wait};
    cond_wait.wait(lock, [&wait_counter]() { return wait_counter.load() == 0; });

    std::for_each(threads.begin(), threads.end(), [](std::thread& thread) { thread.join(); });

    EXPECT_EQ(expected, sum_numbers);
}

TEST(StaticChannelTest, ReadWriteClose)
{
    const int numbers = 10000;
    const std::int64_t expected_sum = 50005000;
    constexpr std::size_t kThreadsToReadFrom = 20;

    msd::static_channel<int, kThreadsToReadFrom> channel{};
    std::atomic<std::int64_t> sum{0};
    std::atomic<std::int64_t> nums{0};

    std::thread writer([&channel]() {
        for (int i = 1; i <= numbers; ++i) {
            channel.write(i);
        }
        channel.close();
    });

    std::vector<std::thread> readers;
    for (std::size_t i = 0; i < kThreadsToReadFrom; ++i) {
        readers.emplace_back([&channel, &sum, &nums]() {
            while (true) {
                int value = 0;

                if (!channel.read(value)) {
                    return;
                }

                sum += value;
                ++nums;
            }
        });
    }

    writer.join();
    for (auto& reader : readers) {
        reader.join();
    }

    EXPECT_EQ(sum, expected_sum);
    EXPECT_EQ(nums, numbers);
}
