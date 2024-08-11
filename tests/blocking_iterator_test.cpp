#include "msd/blocking_iterator.hpp"

#include <gtest/gtest.h>

#include "msd/channel.hpp"

TEST(ChannelIteratorTest, Traits)
{
    using type = int;
    using iterator = msd::blocking_iterator<msd::channel<type>>;
    EXPECT_TRUE((std::is_same<iterator::value_type, type>::value));

    using iterator_traits = std::iterator_traits<iterator>;
    EXPECT_TRUE((std::is_same<iterator_traits::value_type, type>::value));
    EXPECT_TRUE((std::is_same<iterator_traits::iterator_category, std::input_iterator_tag>::value));
}

TEST(ChannelIteratorTest, Dereference)
{
    msd::channel<int> channel;
    msd::blocking_iterator<msd::channel<int>> it{channel};

    int in = 1;
    channel << in;
    in = 2;
    channel << in;

    EXPECT_EQ(1, *it);
    EXPECT_EQ(2, *it);
}

TEST(ChannelIteratorTest, NotEqualStop)
{
    msd::channel<int> channel;
    msd::blocking_iterator<msd::channel<int>> it{channel};

    channel.close();

    EXPECT_FALSE(it != it);
}

TEST(ChannelIteratorTest, NotEqualContinue)
{
    msd::channel<int> channel;
    msd::blocking_iterator<msd::channel<int>> it{channel};

    channel << 1;

    EXPECT_TRUE(it != it);
}
