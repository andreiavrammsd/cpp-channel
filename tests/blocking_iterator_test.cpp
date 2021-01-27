#include "msd/blocking_iterator.hpp"

#include "gtest/gtest.h"
#include "msd/channel.hpp"

TEST(ChannelIteratorTest, Dereference)
{
    msd::channel<int> channel;
    msd::BlockingIterator<msd::channel<int>> it(channel);

    int in = 1;
    in >> channel;
    in = 2;
    in >> channel;

    EXPECT_EQ(1, *it);
    EXPECT_EQ(2, *it);
}

TEST(ChannelIteratorTest, NotEqual)
{
    msd::channel<int> channel;
    msd::BlockingIterator<msd::channel<int>> it(channel);

    EXPECT_TRUE(it != it);
}
