#include <iostream>

#include "msd/channel.hpp"

class Data final {
    int i_{};

   public:
    Data() = default;
    explicit Data(int i) : i_{i} {}

    int getI() const { return i_; }

    Data(const Data& other) noexcept : i_{other.i_} { std::cout << "copy " << i_ << '\n'; }
    Data& operator=(const Data& other)
    {
        if (this != &other) {
            i_ = other.i_;
        }
        std::cout << "copy " << i_ << '\n';

        return *this;
    }

    Data(Data&& other) noexcept : i_{other.i_} { std::cout << "move " << i_ << '\n'; }
    Data& operator=(Data&& other) noexcept
    {
        if (this != &other) {
            i_ = other.i_;
            std::cout << "move " << i_ << '\n';
        }

        return *this;
    }

    ~Data() = default;
};

int main()
{
    msd::channel<Data> ch{10};

    auto in1 = Data{1};
    ch << in1;

    ch << Data{2};

    auto in3 = Data{3};
    ch << std::move(in3);

    for (auto out : ch) {
        std::cout << out.getI() << '\n';

        if (ch.empty()) {
            break;
        }
    }
}
