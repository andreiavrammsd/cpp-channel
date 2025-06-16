#include <msd/channel.hpp>

#include <iostream>

class data final {
    int value_{};

   public:
    data() = default;
    explicit data(int value) : value_{value} {}

    int get_value() const { return value_; }

    data(const data& other) noexcept : value_{other.value_} { std::cout << "copy " << value_ << '\n'; }
    data& operator=(const data& other)
    {
        if (this != &other) {
            value_ = other.value_;
        }
        std::cout << "copy " << value_ << '\n';

        return *this;
    }

    data(data&& other) noexcept : value_{other.value_} { std::cout << "move " << value_ << '\n'; }
    data& operator=(data&& other) noexcept
    {
        if (this != &other) {
            value_ = other.value_;
            std::cout << "move " << value_ << '\n';
        }

        return *this;
    }

    ~data() = default;
};

int main()
{
    msd::channel<data> chan{10};

    auto in1 = data{1};
    chan << in1;

    chan << data{2};

    auto in3 = data{3};
    chan << std::move(in3);

    for (const auto& out : chan) {
        std::cout << out.get_value() << '\n';

        if (chan.empty()) {
            break;
        }
    }
}
