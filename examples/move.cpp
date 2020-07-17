#include <iostream>

#include "channel.hpp"

struct Data {
    int i{};

    Data() = default;
    explicit Data(int i) : i{i} {}

    Data(const Data& other) noexcept : i{other.i} { std::cout << "copy " << i << '\n'; }
    Data& operator=(const Data& other)
    {
        if (this != &other) {
            i = other.i;
        }
        std::cout << "copy " << i << '\n';

        return *this;
    }

    Data(Data&& other) noexcept : i{other.i} { std::cout << "move " << i << '\n'; }
    Data& operator=(Data&& other) noexcept
    {
        if (this != &other) {
            i = other.i;
            std::cout << "move " << i << '\n';
        }

        return *this;
    }
};

int main()
{
    Channel<Data> ch{10};

    auto in1 = Data{1};
    in1 >> ch;

    auto in2 = Data{2};
    std::move(in2) >> ch;

    auto in3 = Data{3};
    std::move(in3) >> ch;

    for (auto out : ch) {
        std::cout << out.i << '\n';

        if (ch.empty()) {
            break;
        }
    }
}
