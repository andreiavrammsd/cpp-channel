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

    msd::blocking_iterator<msd::channel<int>> it{channel};

    channel << 1;

    EXPECT_TRUE(it != it);
}
