#include "benchmark/benchmark.h"
#include "channel.hpp"

/**
    Results on release build with CPU scaling disabled

    2020-07-17 17:11:32
    Running channel_benchmark
    Run on (8 X 4000 MHz CPU s)
    CPU Caches:
      L1 Data 32K (x4)
      L1 Instruction 32K (x4)
      L2 Unified 256K (x4)
      L3 Unified 8192K (x1)
    Load Average: 2.91, 1.63, 1.07
    ---------------------------------------------------------------
    Benchmark                     Time             CPU   Iterations
    ---------------------------------------------------------------
    BM_ChannelWithInt          59.1 ns         59.1 ns     10000000
    BM_ChannelWithString       55.8 ns         55.8 ns     11405133
    BM_ChannelWithStruct       54.9 ns         54.9 ns     12931447
 */

struct Entry {
    int id{};
    std::string label = "label";
};

static void BM_ChannelWithInt(benchmark::State& state)
{
    Channel<int> channel{1};
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
    Channel<std::string> channel{1};
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
    Channel<Entry> channel{1};
    Entry in{};
    Entry out{};
    for (auto _ : state) {
        in >> channel;
        out << channel;
    }
}

BENCHMARK(BM_ChannelWithStruct);

BENCHMARK_MAIN();
