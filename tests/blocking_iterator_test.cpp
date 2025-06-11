#include "msd/blocking_iterator.hpp"

#include <gtest/gtest.h>

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

TEST(BlockingIteratorTest, Dereference)
{
    msd::channel<int> channel;
    channel << 1;
    channel << 2;

    msd::blocking_iterator<msd::channel<int>> it{channel};

    EXPECT_EQ(1, *it);

    ++it;
    EXPECT_EQ(2, *it);
}

TEST(BlockingIteratorTest, NotEqualStop)
{
    msd::channel<int> channel;
    channel.close();

    msd::blocking_iterator<msd::channel<int>> it{channel};
    EXPECT_FALSE(it != it);
}

TEST(BlockingIteratorTest, NotEqualAtEndAndChannelClosed)
{
    msd::channel<int> channel;
    channel.close();

    msd::blocking_iterator<msd::channel<int>> it_begin{channel};
    msd::blocking_iterator<msd::channel<int>> it_end{channel, false};
    EXPECT_FALSE(it_begin != it_end);
}

TEST(BlockingIteratorTest, NotEqualContinue)
{
    msd::channel<int> channel;
    channel << 1;

    msd::blocking_iterator<msd::channel<int>> it{channel};

    EXPECT_FALSE(it != it);
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

TEST(BlockingWriterIteratorTest, Assign)
{
    msd::channel<int> channel{3};
    msd::blocking_writer_iterator<msd::channel<int>> it{channel};

    it = 1;
    it = 2;
    channel.close();

    int out;

    channel >> out;
    EXPECT_EQ(1, out);

    channel >> out;
    EXPECT_EQ(2, out);

    EXPECT_FALSE(channel.read(out));  // channel is closed, no more elements to read
}

TEST(BlockingWriterIteratorTest, Dereference)
{
    msd::channel<int> channel;
    msd::blocking_writer_iterator<msd::channel<int>> it{channel};

    EXPECT_EQ(&(*it), &(it));
}

TEST(BlockingWriterIteratorTest, Increment)
{
    msd::channel<int> channel;
    msd::blocking_writer_iterator<msd::channel<int>> it{channel};

    EXPECT_EQ(&(++it), &(it));
    auto it2 = it++;
    EXPECT_NE(&(it2), &(it));
}

TEST(BlockingWriterIteratorTest, NotEqualStop)
{
    msd::channel<int> channel;
    channel.close();

    msd::blocking_iterator<msd::channel<int>> it{channel};

    EXPECT_FALSE(it != it);
}

TEST(BlockingWriterIteratorTest, NotEqualContinue)
{
    msd::channel<int> channel;
    channel << 1 << 2;

    msd::blocking_iterator<msd::channel<int>> it{channel};
    ++it;

    EXPECT_FALSE(it != it);
}
