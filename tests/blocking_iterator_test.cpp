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
    EXPECT_TRUE((std::is_same<iterator_traits::iterator_category, std::output_iterator_tag>::value));
}

TEST(ChannelIteratorTest, Dereference)
{
    msd::channel<int> channel;
    auto it = channel.begin();

    int in = 1;
    in >> channel;
    in = 2;
    in >> channel;

    EXPECT_EQ(1, *it);
    EXPECT_EQ(1, *it); // no move to next
    ++it; // actually move to next
    EXPECT_EQ(2, *it);
    EXPECT_TRUE(it != channel.end());
    channel.close();
    EXPECT_FALSE(it != channel.end());
}

TEST(ChannelIteratorTest, NotEqualStop)
{
    msd::channel<int> channel;
    auto it = channel.begin();

    channel.close();

    EXPECT_FALSE(it != channel.end());
}

TEST(ChannelIteratorTest, NotEqualContinue)
{
    msd::channel<int> channel;
    auto it = channel.begin();

    1 >> channel;

    EXPECT_TRUE(it != channel.end());
}
