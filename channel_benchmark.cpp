#include "benchmark/benchmark.h"
#include "channel.hpp"

/*
 ---------------------------------------------------------------
 Benchmark                     Time             CPU   Iterations
 ---------------------------------------------------------------
 BM_ChannelWithInt           121 ns          121 ns      5628541
 BM_ChannelWithString        235 ns          235 ns      2962239
 BM_ChannelWithStruct        246 ns          246 ns      2864420
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
