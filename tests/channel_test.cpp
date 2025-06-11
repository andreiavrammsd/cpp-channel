#include "msd/channel.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <future>
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
    channel << in;

    const int cin = 3;
    channel << cin;

    channel << 2 << 4;

    int out = 0;

    channel >> out;
    EXPECT_EQ(1, out);

    channel >> out;
    EXPECT_EQ(3, out);

    channel >> out;
    EXPECT_EQ(2, out);

    channel >> out;
    EXPECT_EQ(4, out);
}

TEST(ChannelTest, WriteAndRead)
{
    msd::channel<int> channel;

    int in = 1;
    EXPECT_TRUE(channel.write(in));

    const int cin = 3;
    EXPECT_TRUE(channel.write(cin));

    channel.close();
    EXPECT_FALSE(channel.write(2));

    int out = 0;

    EXPECT_TRUE(channel.read(out));
    EXPECT_EQ(1, out);

    EXPECT_TRUE(channel.read(out));
    EXPECT_EQ(3, out);

    EXPECT_FALSE(channel.read(out));
}

TEST(ChannelTest, PushAndFetchWithBufferedChannel)
{
    msd::channel<int> channel{2};

    auto push = [&channel]() {
        channel << 1;
        channel << 2;
        channel << 3;
    };

    auto read = [&channel]() {
        // Wait before reading to test the case where the channel is full and waiting
        // for the reader to read some items.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        int out = 0;

        channel >> out;
        EXPECT_EQ(1, out);

        channel >> out;
        EXPECT_EQ(2, out);

        channel >> out;
        EXPECT_EQ(3, out);
    };

    std::thread push_thread{push};
    std::thread read_thread{read};
    push_thread.join();
    read_thread.join();
}

TEST(ChannelTest, PushAndFetchMultiple)
{
    msd::channel<int> channel;

    int a = 1;
    const int b = 3;
    channel << a << 2 << b << std::move(a);

    int out = 0;
    int out2 = 0;

    channel >> out;
    EXPECT_EQ(1, out);

    channel >> out;
    EXPECT_EQ(2, out);

    channel >> out >> out2;
    EXPECT_EQ(3, out);
    EXPECT_EQ(1, out2);
}

TEST(ChannelTest, PushByMoveAndFetch)
{
    msd::channel<std::string> channel;

    std::string in = "abc";
    channel << std::move(in);

    channel << std::string{"def"};

    std::string out{};
    channel >> out;
    EXPECT_EQ("abc", out);

    channel >> out;
    EXPECT_EQ("def", out);
}

TEST(ChannelTest, size)
{
    msd::channel<int> channel;
    EXPECT_EQ(0, channel.size());

    int in = 1;
    channel << in;
    EXPECT_EQ(1, channel.size());

    channel >> in;
    EXPECT_EQ(0, channel.size());
}

TEST(ChannelTest, empty)
{
    msd::channel<int> channel;
    EXPECT_TRUE(channel.empty());

    int in = 1;
    channel << in;
    EXPECT_FALSE(channel.empty());

    channel >> in;
    EXPECT_TRUE(channel.empty());
}

TEST(ChannelTest, close)
{
    msd::channel<int> channel;
    EXPECT_FALSE(channel.closed());

    int in = 1;
    channel << in;

    channel.close();
    EXPECT_TRUE(channel.closed());

    int out = 0;
    channel >> out;
    EXPECT_EQ(1, out);
    EXPECT_NO_THROW(channel >> out);

    EXPECT_THROW(channel << in, msd::closed_channel);
    EXPECT_THROW(channel << std::move(in), msd::closed_channel);
}

TEST(ChannelTest, drained)
{
    msd::channel<int> channel;
    EXPECT_FALSE(channel.drained());

    int in = 1;
    channel << in;

    channel.close();
    EXPECT_FALSE(channel.drained());

    int out = 0;
    channel >> out;
    EXPECT_EQ(1, out);
    EXPECT_TRUE(channel.drained());
}

TEST(ChannelTest, Iterator)
{
    msd::channel<int> channel;

    channel << 1;

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
            channel >> out;

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
        channel << i;

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

TEST(ChannelTest, ReadWriteClose)
{
    const int numbers = 10000;
    const std::int64_t expected_sum = 50005000;
    constexpr std::size_t kThreadsToReadFrom = 20;

    msd::channel<int> channel{kThreadsToReadFrom};
    std::atomic<std::int64_t> sum{0};
    std::atomic<std::int64_t> nums{0};

    std::thread writer([&channel]() {
        for (int i = 1; i <= numbers; ++i) {
            channel << i;
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

class MovableOnly {
   public:
    explicit MovableOnly(int v) : value(v) {}

    MovableOnly() = default;

    MovableOnly(const MovableOnly&)
    {
        std::cout << "Copy constructor should not be called for MovableOnly";
        std::abort();
    }

    MovableOnly(MovableOnly&& other) noexcept : value{std::move(other.value)} { other.value = 0; }

    MovableOnly& operator=(const MovableOnly&)
    {
        std::cout << "Copy assignment should not be called for MovableOnly";
        std::abort();
    }

    MovableOnly& operator=(MovableOnly&& other) noexcept
    {
        if (this != &other) {
            value = other.value;
            other.value = 0;
        }

        return *this;
    }

    int getValue() const { return value; }

   private:
    int value{0};
};

template msd::blocking_writer_iterator<msd::channel<int>>& msd::blocking_writer_iterator<msd::channel<int>>::operator=
    <int>(int&&);

template msd::blocking_writer_iterator<msd::channel<int>>& msd::blocking_writer_iterator<msd::channel<int>>::operator=
    <const int&>(const int&);

template bool msd::channel<int>::write(int&&);
template bool msd::channel<int>::write(const int&);

TEST(ChannelTest, Transform)
{
    const int numbers = 100;
    const std::int64_t expected_sum = 5050 * 2;
    std::atomic<std::int64_t> sum{0};
    std::atomic<std::int64_t> nums{0};

    msd::channel<MovableOnly> input_chan{30};
    msd::channel<int> output_chan{10};

    // Send to channel input channel
    const auto writer = [&input_chan]() {
        for (int i = 1; i <= numbers; ++i) {
            input_chan.write(MovableOnly{i});
        }
        input_chan.close();
    };

    // Transform input channel values from MovableOnly to int by multiplying by 2 and write to output channel
    const auto double_transformer = [&input_chan, &output_chan]() {
        std::transform(input_chan.begin(), input_chan.end(), msd::back_inserter(output_chan),
                       [](auto&& value) -> int { return value.getValue() * 2; });
        output_chan.close();
    };

    // Read from output channel
    const auto reader = [&output_chan, &sum, &nums]() {
        for (auto&& out : output_chan) {  // blocking until channel is drained (closed and empty)
            sum += out;
            ++nums;
        }
    };

    // Create async tasks for reading, transforming, and writing
    const auto reader_task_1 = std::async(std::launch::async, reader);
    const auto reader_task_2 = std::async(std::launch::async, reader);
    const auto writer_task = std::async(std::launch::async, writer);
    const auto transformer_task = std::async(std::launch::async, double_transformer);

    writer_task.wait();
    transformer_task.wait();
    reader_task_1.wait();
    reader_task_2.wait();

    EXPECT_EQ(sum, expected_sum);
    EXPECT_EQ(nums, numbers);
}
