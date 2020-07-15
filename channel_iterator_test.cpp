#include "channel_iterator.hpp"

#include "channel.hpp"
#include "gtest/gtest.h"

class ChannelIteratorTest : public ::testing::Test {
};

TEST_F(ChannelIteratorTest, Dereference)
{
    Channel<int> channel;
    channel_iterator<int> it(channel);

    int in = 1;
    in >> channel;
    in = 2;
    in >> channel;

    EXPECT_EQ(*it, 1);
    EXPECT_EQ(*it, 2);
}

TEST_F(ChannelIteratorTest, NotEqual)
{
    Channel<int> channel;
    channel_iterator<int> it(channel);

    EXPECT_TRUE(it != it);
}
