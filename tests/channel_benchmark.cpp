#include <benchmark/benchmark.h>

#include <string>

#include "msd/channel.hpp"

/**
    Results on release build with CPU scaling disabled
    c++ (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0

    2025-06-13T00:17:30+03:00
    Running ./tests/channel_benchmark
    Run on (8 X 3999.91 MHz CPU s)
    CPU Caches:
    L1 Data 32 KiB (x4)
    L1 Instruction 32 KiB (x4)
    L2 Unified 256 KiB (x4)
    L3 Unified 8192 KiB (x1)
    Load Average: 2.65, 1.61, 1.50
    ----------------------------------------------------------------------
    Benchmark                            Time             CPU   Iterations
    ----------------------------------------------------------------------
    BM_ChannelWithQueueStorage       42602 ns        42598 ns        16407
    BM_ChannelWithVectorStorage      42724 ns        42723 ns        16288
    BM_ChannelWithArrayStorage       51332 ns        51328 ns        11776
 */

static void BM_ChannelWithQueueStorage(benchmark::State& state)
{
    msd::channel<std::string, msd::queue_storage<std::string>> channel{10};

    std::string input(1000000, 'x');
    std::string out = "";
    out.resize(input.size());

    for (auto _ : state) {
        benchmark::DoNotOptimize(channel << input);
        benchmark::DoNotOptimize(channel >> out);
    }
}

BENCHMARK(BM_ChannelWithQueueStorage);

static void BM_ChannelWithVectorStorage(benchmark::State& state)
{
    msd::channel<std::string, msd::vector_storage<std::string>> channel{10};

    std::string input(1000000, 'x');
    std::string out = "";
    out.resize(input.size());

    for (auto _ : state) {
        benchmark::DoNotOptimize(channel << input);
        benchmark::DoNotOptimize(channel >> out);
    }
}

BENCHMARK(BM_ChannelWithVectorStorage);

static void BM_ChannelWithArrayStorage(benchmark::State& state)
{
    msd::channel<std::string, msd::array_storage<std::string, 10>> channel{};

    std::string input(1000000, 'x');
    std::string out = "";
    out.resize(input.size());

    for (auto _ : state) {
        benchmark::DoNotOptimize(channel << input);
        benchmark::DoNotOptimize(channel >> out);
    }
}

BENCHMARK(BM_ChannelWithArrayStorage);

BENCHMARK_MAIN();
