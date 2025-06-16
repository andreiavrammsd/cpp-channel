#include <msd/channel.hpp>

#include <chrono>
#include <cstddef>
#include <future>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

class semaphore {
   public:
    explicit semaphore(std::size_t limit) : chan_{limit} {}

    void acquire() { chan_ << empty_; }

    void release() { chan_ >> empty_; }

   private:
    struct empty {};
    [[no_unique_address]] empty empty_{};
    msd::channel<empty> chan_;
};

int simulate_heavy_computation(const int value)
{
    const int result = value * 2;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    return result;
};

int main()
{
    semaphore sem{2};

    std::vector<std::future<int>> futures;

    for (int i = 1; i <= 10; ++i) {
        const auto worker = [&sem](const int value) {
            sem.acquire();
            const int result = simulate_heavy_computation(value);
            sem.release();

            return result;
        };

        futures.push_back(std::async(worker, i));
    }

    std::cout << "Waiting for result...\n";
    const int result = std::accumulate(futures.begin(), futures.end(), 0,
                                       [](int acc, std::future<int>& future) { return acc + future.get(); });
    std::cout << "Result: " << result << "\n";
}
