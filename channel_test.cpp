#include "channel.h"

#include "gtest/gtest.h"

class ChannelTest : public ::testing::Test {
protected:
    Channel<int> channel{};
};

TEST_F(ChannelTest, test) {
    ASSERT_EQ(channel.size(), 0);

    int in = 2;
    in >> channel;

    ASSERT_EQ(channel.size(), 1);

    int out;
    out << channel;

    ASSERT_EQ(out, in);

    ASSERT_EQ(channel.size(), 0);
}
