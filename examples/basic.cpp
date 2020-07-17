#include <iostream>

#include "../channel.hpp"

int main()
{
    Channel<int> ch{10};

    int in;

    in = 1;
    in >> ch;

    in = 2;
    in >> ch;

    in = 3;
    in >> ch;

    for (auto out : ch) {
        std::cout << out << '\n';

        if (ch.empty()) {
            break;
        }
    }
}
