#include "benchmark/benchmark.h"
#include "msd/channel.hpp"

/**
    Results on release build (GCC 10) with CPU scaling disabled

    2022-03-20T23:00:06+02:00
    Running channel_benchmark
    Run on (8 X 3595.91 MHz CPU s)
    CPU Caches:
      L1 Data 32 KiB (x4)
      L1 Instruction 32 KiB (x4)
      L2 Unified 256 KiB (x4)
      L3 Unified 8192 KiB (x1)
    Load Average: 3.29, 2.79, 2.33
    ---------------------------------------------------------------
    Benchmark                     Time             CPU   Iterations
    ---------------------------------------------------------------
    BM_ChannelWithInt          42.0 ns         42.0 ns     15997933
    BM_ChannelWithString       46.3 ns         46.3 ns     15363996
    BM_ChannelWithStruct       47.6 ns         47.6 ns     14817823
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
        in >> channel;
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
        in >> channel;
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
        in >> channel;
        out << channel;
    }
}

BENCHMARK(BM_ChannelWithStruct);

BENCHMARK_MAIN();
