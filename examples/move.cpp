#include <msd/channel.hpp>

#include <iostream>

class Data final {
    int value_{};

   public:
    Data() = default;
    explicit Data(int value) : value_{value} {}

    int get_value() const { return value_; }

    Data(const Data& other) noexcept : value_{other.value_} { std::cout << "copy " << value_ << '\n'; }
    Data& operator=(const Data& other)
    {
        if (this != &other) {
            value_ = other.value_;
        }
        std::cout << "copy " << value_ << '\n';

        return *this;
    }

    Data(Data&& other) noexcept : value_{other.value_} { std::cout << "move " << value_ << '\n'; }
    Data& operator=(Data&& other) noexcept
    {
        if (this != &other) {
            value_ = other.value_;
            std::cout << "move " << value_ << '\n';
        }

        return *this;
    }

    ~Data() = default;
};

int main()
{
    msd::channel<Data> chan{10};

    auto in1 = Data{1};
    chan << in1;

    chan << Data{2};

    auto in3 = Data{3};
    chan << std::move(in3);

    for (const auto& out : chan) {
        std::cout << out.get_value() << '\n';

        if (chan.empty()) {
            break;
        }
    }
}
