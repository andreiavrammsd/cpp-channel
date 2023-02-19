#include "msd/channel.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

TEST(ChannelTest, Traits)
{
    using type = int;
    using channel = msd::channel<type>;
    EXPECT_TRUE((std::is_same<channel::value_type, type>::value));

    using iterator = msd::blocking_iterator<msd::channel<type>>;
    EXPECT_TRUE((std::is_same<channel::iterator, iterator>::value));

    EXPECT_TRUE((std::is_same<channel::size_type, std::size_t>::value));
}

TEST(ChannelTest, PushAndFetch)
{
    msd::channel<int> channel;

    int in = 1;
    in >> channel;

    const int cin = 3;
    cin >> channel;

    2 >> channel;

    int out = 0;

    out << channel;
    EXPECT_EQ(1, out);

    out << channel;
    EXPECT_EQ(3, out);

    out << channel;
    EXPECT_EQ(2, out);
}

TEST(ChannelTest, PushByMoveAndFetch)
{
    msd::channel<std::string> channel;

    std::string in = "abc";
    std::move(in) >> channel;

    std::string{"def"} >> channel;

    std::string out{};
    out << channel;
    EXPECT_EQ("abc", out);

    out << channel;
    EXPECT_EQ("def", out);
}

TEST(ChannelTest, size)
{
    msd::channel<int> channel;
    EXPECT_EQ(0, channel.size());

    int in = 1;
    in >> channel;
    EXPECT_EQ(1, channel.size());

    in << channel;
    EXPECT_EQ(0, channel.size());
}

TEST(ChannelTest, empty)
{
    msd::channel<int> channel;
    EXPECT_TRUE(channel.empty());

    int in = 1;
    in >> channel;
    EXPECT_FALSE(channel.empty());

    in << channel;
    EXPECT_TRUE(channel.empty());
}

TEST(ChannelTest, close)
{
    msd::channel<int> channel;
    EXPECT_FALSE(channel.closed());

    int in = 1;
    in >> channel;

    channel.close();
    EXPECT_TRUE(channel.closed());

    int out = 0;
    out << channel;
    EXPECT_EQ(1, out);
    EXPECT_NO_THROW(out << channel);

    EXPECT_THROW(in >> channel, msd::closed_channel);
    EXPECT_THROW(std::move(in) >> channel, msd::closed_channel);
}

TEST(ChannelTest, Iterator)
{
    msd::channel<int> channel;

    1 >> channel;

    for (auto it = channel.begin(); it != channel.end();) {
        EXPECT_EQ(1, *it);
        break;
    }
}

TEST(ChannelTest, Multithreading)
{
    const int numbers = 10000;
    const std::int64_t expected = 50005000;
    constexpr std::size_t kThreadsToReadFrom = 100;

    msd::channel<int> channel{10};

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
            out << channel;

            sum_numbers += out;
            ++count_numbers;
        }
        --wait_counter;
        cond_wait.notify_one();
    };

    std::vector<std::thread> threads{};
    for (std::size_t i = 0U; i < kThreadsToReadFrom; ++i) {
        threads.emplace_back(std::thread{worker});
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
    cond_wait.wait(lock, [&wait_counter]() {
        auto items = wait_counter.load();
        return items == 0;
    });

    std::for_each(threads.begin(), threads.end(), [](std::thread& thread) { thread.join(); });

    EXPECT_EQ(expected, sum_numbers);
}
