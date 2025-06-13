#include <benchmark/benchmark.h>
#include <msd/channel.hpp>

#include <string>

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
    ------------------------------------------------------------------------------
    Benchmark                           Time            CPU             Iterations
    ------------------------------------------------------------------------------
    bm_channel_with_queue_storage       42602 ns        42598 ns        16407
    bm_channel_with_vector_storage      42724 ns        42723 ns        16288
    bm_channel_with_vector_storage      51332 ns        51328 ns        11776
 */

static void bm_channel_with_queue_storage(benchmark::State& state)
{
    msd::channel<std::string, msd::queue_storage<std::string>> channel{10};

    std::string input(1000000, 'x');
    std::string out{};
    out.resize(input.size());

    for (auto _ : state) {
        benchmark::DoNotOptimize(channel << input);
        benchmark::DoNotOptimize(channel >> out);
    }
}

BENCHMARK(bm_channel_with_queue_storage);

static void bm_channel_with_vector_storage(benchmark::State& state)
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

BENCHMARK(bm_channel_with_vector_storage);

static void bm_channel_with_array_storage(benchmark::State& state)
{
    msd::channel<std::string, msd::array_storage<std::string, 10>> channel{};

    std::string input(1000000, 'x');
    std::string out;
    out.resize(input.size());

    for (auto _ : state) {
        benchmark::DoNotOptimize(channel << input);
        benchmark::DoNotOptimize(channel >> out);
    }
}

BENCHMARK(bm_channel_with_array_storage);

BENCHMARK_MAIN();
