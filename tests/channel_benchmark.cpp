#include <benchmark/benchmark.h>

#include "msd/channel.hpp"

/**
    Results on release build (GCC 10) with CPU scaling disabled

    2023-02-05T20:35:06+02:00
    Running channel_benchmark
    Run on (8 X 3595.91 MHz CPU s)
    CPU Caches:
      L1 Data 32 KiB (x4)
      L1 Instruction 32 KiB (x4)
      L2 Unified 256 KiB (x4)
      L3 Unified 8192 KiB (x1)
    Load Average: 0.78, 0.82, 0.84
    ---------------------------------------------------------------
    Benchmark                     Time             CPU   Iterations
    ---------------------------------------------------------------
    BM_ChannelWithInt          31.3 ns         31.3 ns     21684083
    BM_ChannelWithString       37.1 ns         37.1 ns     18832698
    BM_ChannelWithStruct       37.6 ns         37.6 ns     18625765
 */

struct Entry {
    int id{};
    std::string label = "label";
};

static void BM_ChannelWithInt(benchmark::State& state)
{
    msd::channel<int> channel{1};
    int in = 1;
    int out = 0;
    for (auto _ : state) {
        channel << in;
        out << channel;
    }
}

BENCHMARK(BM_ChannelWithInt);

static void BM_ChannelWithString(benchmark::State& state)
{
    msd::channel<std::string> channel{1};
    std::string in = "input";
    std::string out;
    for (auto _ : state) {
        channel << in;
        out << channel;
    }
}

BENCHMARK(BM_ChannelWithString);

static void BM_ChannelWithStruct(benchmark::State& state)
{
    msd::channel<Entry> channel{1};
    Entry in{};
    Entry out{};
    for (auto _ : state) {
        channel << in;
        out << channel;
    }
}

BENCHMARK(BM_ChannelWithStruct);

BENCHMARK_MAIN();
