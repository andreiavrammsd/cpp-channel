#include "msd/channel.hpp"

#include <benchmark/benchmark.h>

#include <string>
#include <thread>

/**
    Results on release build with CPU scaling disabled
    g++ (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0

    2025-06-14T22:36:32+03:00
    Running ./benchmarks/channel_benchmark
    Run on (8 X 3999.7 MHz CPU s)
    CPU Caches:
    L1 Data 32 KiB (x4)
    L1 Instruction 32 KiB (x4)
    L2 Unified 256 KiB (x4)
    L3 Unified 8192 KiB (x1)
    Load Average: 1.22, 1.13, 1.21
    -----------------------------------------------------------------------------
    Benchmark                                   Time             CPU   Iterations
    -----------------------------------------------------------------------------
    channel_with_queue_storage_mean     466969277 ns    428207411 ns           10
    channel_with_queue_storage_median   433695208 ns    363801277 ns           10
    channel_with_queue_storage_stddev    72558997 ns     99239885 ns           10
    channel_with_queue_storage_cv           15.54 %         23.18 %            10
    channel_with_vector_storage_mean       461215 ns       405569 ns           10
    channel_with_vector_storage_median     438756 ns       378237 ns           10
    channel_with_vector_storage_stddev      70937 ns        88753 ns           10
    channel_with_vector_storage_cv          15.38 %         21.88 %            10
    channel_with_array_storage_mean           233 ns          221 ns           10
    channel_with_array_storage_median         210 ns          194 ns           10
    channel_with_array_storage_stddev        52.1 ns         57.8 ns           10
    channel_with_array_storage_cv           22.35 %         26.22 %            10
 */

template <typename Channel>
std::thread create_producer(Channel& channel)
{
    return std::thread([&] {
        for (size_t i = 0; i < 100000; ++i) {
            std::string input(i, 'c');
            channel << std::move(input);
        }
        channel.close();
    });
}

static constexpr std::size_t channel_capacity = 1024;

static void channel_with_queue_storage(benchmark::State& state)
{
    msd::channel<std::string, msd::queue_storage<std::string>> channel{channel_capacity};
    auto producer = create_producer(channel);

    for (auto _ : state) {
        for (auto value : channel) {
            benchmark::DoNotOptimize(value);
        }
    }

    producer.join();
}

BENCHMARK(channel_with_queue_storage);

static void channel_with_vector_storage(benchmark::State& state)
{
    msd::channel<std::string, msd::vector_storage<std::string>> channel{1024};
    auto producer = create_producer(channel);

    for (auto _ : state) {
        for (auto value : channel) {
            benchmark::DoNotOptimize(value);
        }
    }

    producer.join();
}

BENCHMARK(channel_with_vector_storage);

static void channel_with_array_storage(benchmark::State& state)
{
    msd::channel<std::string, msd::array_storage<std::string, channel_capacity>> channel{};
    auto producer = create_producer(channel);

    for (auto _ : state) {
        for (auto value : channel) {
            benchmark::DoNotOptimize(value);
        }
    }

    producer.join();
}

BENCHMARK(channel_with_array_storage);

BENCHMARK_MAIN();
