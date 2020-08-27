#include "blocking_iterator.hpp"

#include "channel.hpp"
#include "gtest/gtest.h"

TEST(ChannelIteratorTest, Dereference)
{
    Channel<int> channel;
    BlockingIterator<Channel<int>> it(channel);

    int in = 1;
    in >> channel;
    in = 2;
    in >> channel;

    EXPECT_EQ(1, *it);
    EXPECT_EQ(2, *it);
}

TEST(ChannelIteratorTest, NotEqual)
{
    Channel<int> channel;
    BlockingIterator<Channel<int>> it(channel);

    EXPECT_TRUE(it != it);
}
