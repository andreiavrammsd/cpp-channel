#include <msd/channel.hpp>

#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

class data final {
   public:
    static std::size_t copies_;
    static std::size_t moves_;

    data() = default;
    explicit data(int value) : value_{value} {}

    int get_value() const { return value_; }

    data(const data& other) noexcept : value_{other.value_}
    {
        std::cout << "copy " << value_ << '\n';
        ++copies_;
    }

    data& operator=(const data& other)
    {
        if (this != &other) {
            value_ = other.value_;
            std::cout << "copy " << value_ << '\n';
            ++copies_;
        }

        return *this;
    }

    data(data&& other) noexcept : value_{other.value_}
    {
        std::cout << "move " << value_ << '\n';
        ++moves_;
    }

    data& operator=(data&& other) noexcept
    {
        if (this != &other) {
            value_ = other.value_;
            std::cout << "move " << value_ << '\n';
            ++moves_;
        }

        return *this;
    }

    virtual ~data() = default;

   private:
    int value_{};
};

std::size_t data::copies_{};
std::size_t data::moves_{};

// Copy and move semantics with a user-defined type.

int main()
{
    msd::channel<data> chan{10};

    // l-value: will be copied
    const auto in1 = data{1};
    chan << in1;

    // r-value: will be moved
    chan << data{2};

    // l-value -> std::move -> r-value: will be moved
    auto in3 = data{3};
    chan << std::move(in3);

    std::vector<int> actual;

    // Each value will be moved when read
    for (const data& out : chan) {
        std::cout << out.get_value() << '\n';

        actual.push_back(out.get_value());

        if (chan.empty()) {
            break;
        }
    }

    const std::vector<int> expected{
        1,
        2,
        3,
    };

    if (actual != expected) {
        std::cerr << "Error: got: ";
        for (const int value : actual) {
            std::cerr << value << ", ";
        }

        std::cerr << ", expected: ";
        for (const int value : expected) {
            std::cerr << value << ", ";
        }
        std::cerr << '\n';

        std::terminate();
    }

    // 1 copy when in1 was written
    constexpr std::size_t expected_copies = 1;

    if (data::copies_ != expected_copies) {
        std::cerr << "Error: copies: " << data::copies_ << ", expected: " << expected_copies << '\n';
        std::terminate();
    }

    // 1 move when the second value was written
    // 1 move when in3 was written
    // 3 moves when the 3 values were read
    constexpr std::size_t expected_moves = 5;

    if (data::moves_ != expected_moves) {
        std::cerr << "Error: moves: " << data::moves_ << ", expected: " << expected_moves << '\n';
        std::terminate();
    }
}
