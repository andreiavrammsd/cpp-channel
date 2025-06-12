#include "msd/blocking_iterator.hpp"

#include <gtest/gtest.h>

#include <thread>
#include <vector>

#include "msd/channel.hpp"

TEST(BlockingIteratorTest, Traits)
{
    using type = int;
    using iterator = msd::blocking_iterator<msd::channel<type>>;
    EXPECT_TRUE((std::is_same<iterator::value_type, type>::value));

    using iterator_traits = std::iterator_traits<iterator>;
    EXPECT_TRUE((std::is_same<iterator_traits::value_type, type>::value));
    EXPECT_TRUE((std::is_same<iterator_traits::iterator_category, std::input_iterator_tag>::value));
}

TEST(BlockingIteratorTest, ReadFromChannelInOrder)
{
    msd::channel<int> channel{10};
    channel.write(1);
    channel.write(2);
    channel.write(3);
    channel.close();

    msd::blocking_iterator it{channel};
    msd::blocking_iterator end{channel, true};

    std::vector<int> results;
    while (it != end) {
        results.push_back(*it);
        ++it;
    }

    EXPECT_EQ(results, (std::vector<int>{1, 2, 3}));
}

TEST(BlockingIteratorTest, EmptyChannelClosesGracefully)
{
    msd::channel<int> channel;
    channel.close();

    msd::blocking_iterator it{channel};
    msd::blocking_iterator end{channel, true};

    EXPECT_FALSE(it != end);
}

TEST(BlockingWriterIteratorTest, Traits)
{
    using type = int;
    using iterator = msd::blocking_writer_iterator<msd::channel<type>>;
    EXPECT_TRUE((std::is_same<iterator::value_type, type>::value));

    using iterator_traits = std::iterator_traits<iterator>;
    EXPECT_TRUE((std::is_same<iterator_traits::value_type, type>::value));
    EXPECT_TRUE((std::is_same<iterator_traits::iterator_category, std::output_iterator_tag>::value));
}

TEST(BlockingWriterIteratorTest, WriteToChannelUsingBackInserter)
{
    msd::channel<int> channel{10};

    std::thread producer([&channel]() {
        auto out = msd::back_inserter(channel);
        *out = 10;
        *out = 20;
        *out = 30;
        channel.close();
        *out = 40;  // ignored because channel is closed
    });

    std::vector<int> results;
    msd::blocking_iterator it{channel};
    msd::blocking_iterator end{channel, true};

    while (it != end) {
        results.push_back(*it);
        ++it;
    }

    producer.join();
    EXPECT_EQ(results, (std::vector<int>{10, 20, 30}));
}
