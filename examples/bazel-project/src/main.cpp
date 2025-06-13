#include <msd/channel.hpp>

#include <cstddef>
#include <iostream>

int main()
{
    constexpr std::size_t channel_size = 10;
    msd::channel<int> chan{channel_size};

    int input{};

    try {
        input = 1;
        chan << input;

        input = 2;
        chan << input;

        input = 3;
        chan << input;
    }
    catch (const msd::closed_channel& ex) {
        std::cout << ex.what() << '\n';
        return 1;
    }

    for (auto out : chan) {
        std::cout << out << '\n';

        if (chan.empty()) {
            break;
        }
    }
}
