#include <msd/channel.hpp>

#include <iostream>

int main()
{
    constexpr std::size_t capacity = 10;
    msd::channel<int> channel{capacity};

    channel << 1;

    channel << 2 << 3;

    for (auto out : channel) {
        std::cout << out << '\n';

        if (channel.empty()) {
            break;
        }
    }
}
