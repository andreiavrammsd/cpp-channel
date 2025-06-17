#include "msd/channel.hpp"

#include <benchmark/benchmark.h>

#include <array>
#include <cstddef>
#include <string>
#include <thread>

// clang-format off
/**
    Results on release build with CPU scaling disabled
    c++ (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0

    2025-06-17T19:39:12+03:00
    Running ./benchmarks/channel_benchmark
    Run on (8 X 4000.11 MHz CPU s)
    CPU Caches:
    L1 Data 32 KiB (x4)
    L1 Instruction 32 KiB (x4)
    L2 Unified 256 KiB (x4)
    L3 Unified 8192 KiB (x1)
    Load Average: 1.25, 1.24, 1.17
    ------------------------------------------------------------------------------------------------------------------------------------------------------------
    Benchmark                                                                                                                  Time             CPU   Iterations
    ------------------------------------------------------------------------------------------------------------------------------------------------------------
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<100000>>_mean                          56.0 ns         31.7 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<100000>>_median                        55.1 ns         31.2 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<100000>>_stddev                        1.98 ns        0.973 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<100000>>_cv                            3.53 %          3.07 %            10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<100000>>_max                           60.6 ns         33.9 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<100000>>_mean                    974389204 ns    511590696 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<100000>>_median                  970045807 ns    506940233 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<100000>>_stddev                   19792287 ns     21877096 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<100000>>_cv                           2.03 %          4.28 %            10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<100000>>_max                    1014383413 ns    558581282 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<100000>>_mean         43.6 ns         26.5 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<100000>>_median       43.4 ns         26.4 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<100000>>_stddev       1.04 ns        0.762 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<100000>>_cv           2.39 %          2.88 %            10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<100000>>_max          45.8 ns         27.5 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<1000>>_mean                            21.0 ns         20.6 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<1000>>_median                          21.0 ns         20.6 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<1000>>_stddev                         0.056 ns        0.052 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<1000>>_cv                              0.27 %          0.25 %            10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<1000>>_max                             21.1 ns         20.7 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<1000>>_mean                           21.8 ns         21.5 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<1000>>_median                         21.7 ns         21.4 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<1000>>_stddev                        0.413 ns        0.396 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<1000>>_cv                             1.90 %          1.84 %            10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<1000>>_max                            22.6 ns         22.3 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<1000>>_mean           18.7 ns         18.7 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<1000>>_median         18.7 ns         18.7 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<1000>>_stddev        0.051 ns        0.049 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<1000>>_cv             0.27 %          0.26 %            10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<1000>>_max            18.8 ns         18.8 ns           10
    bench_dynamic_storage<data, msd::queue_storage<data>, struct_input>_mean                                                19.5 ns         18.9 ns           10
    bench_dynamic_storage<data, msd::queue_storage<data>, struct_input>_median                                              19.5 ns         18.9 ns           10
    bench_dynamic_storage<data, msd::queue_storage<data>, struct_input>_stddev                                             0.053 ns        0.063 ns           10
    bench_dynamic_storage<data, msd::queue_storage<data>, struct_input>_cv                                                  0.27 %          0.33 %            10
    bench_dynamic_storage<data, msd::queue_storage<data>, struct_input>_max                                                 19.6 ns         19.1 ns           10
    bench_dynamic_storage<data, msd::vector_storage<data>, struct_input>_mean                                               38.3 ns         37.1 ns           10
    bench_dynamic_storage<data, msd::vector_storage<data>, struct_input>_median                                             37.4 ns         36.2 ns           10
    bench_dynamic_storage<data, msd::vector_storage<data>, struct_input>_stddev                                             2.41 ns         2.37 ns           10
    bench_dynamic_storage<data, msd::vector_storage<data>, struct_input>_cv                                                 6.28 %          6.38 %            10
    bench_dynamic_storage<data, msd::vector_storage<data>, struct_input>_max                                                44.7 ns         43.4 ns           10
    bench_static_storage<data, msd::array_storage<data, channel_capacity>, struct_input>_mean                               17.7 ns         17.5 ns           10
    bench_static_storage<data, msd::array_storage<data, channel_capacity>, struct_input>_median                             17.6 ns         17.4 ns           10
    bench_static_storage<data, msd::array_storage<data, channel_capacity>, struct_input>_stddev                            0.070 ns        0.082 ns           10
    bench_static_storage<data, msd::array_storage<data, channel_capacity>, struct_input>_cv                                 0.40 %          0.47 %            10
    bench_static_storage<data, msd::array_storage<data, channel_capacity>, struct_input>_max                                17.8 ns         17.6 ns           10
 */
// clang-format on

static constexpr std::size_t channel_capacity = 1024;
static constexpr std::size_t number_of_inputs = 100000;

template <std::size_t Size>
struct string_input {
    static std::string make() { return std::string(Size, 'c'); }
};

struct data {
    std::array<int, 1000> data{};
};

struct struct_input {
    static data make() { return data{}; }
};

template <typename T, typename Storage, typename Input>
static void bench_dynamic_storage(benchmark::State& state)
{
    msd::channel<T, Storage> channel{channel_capacity};
    const auto input = Input::make();

    std::thread producer([&] {
        for (std::size_t i = 0; i < number_of_inputs; ++i) {
            channel << input;
        }
        channel.close();
    });

    for (auto _ : state) {
        for (auto& value : channel) {
            volatile auto* do_not_optimize = &value;
            (void)do_not_optimize;
        }
    }

    producer.join();
}

template <typename T, typename Storage, typename Input>
static void bench_static_storage(benchmark::State& state)
{
    msd::channel<T, Storage> channel{};
    const auto input = Input::make();

    std::thread producer([&] {
        for (std::size_t i = 0; i < number_of_inputs; ++i) {
            channel << input;
        }
        channel.close();
    });

    for (auto _ : state) {
        for (auto& value : channel) {
            volatile auto* do_not_optimize = &value;
            (void)do_not_optimize;
        }
    }

    producer.join();
}

#define BENCH(...)                                                                               \
    BENCHMARK_TEMPLATE(__VA_ARGS__)->ComputeStatistics("max", [](const std::vector<double>& v) { \
        return *std::max_element(v.begin(), v.end());                                            \
    })

BENCH(bench_dynamic_storage, std::string, msd::queue_storage<std::string>, string_input<100000>);
BENCH(bench_dynamic_storage, std::string, msd::vector_storage<std::string>, string_input<100000>);
BENCH(bench_static_storage, std::string, msd::array_storage<std::string, channel_capacity>, string_input<100000>);

BENCH(bench_dynamic_storage, std::string, msd::queue_storage<std::string>, string_input<1000>);
BENCH(bench_dynamic_storage, std::string, msd::vector_storage<std::string>, string_input<1000>);
BENCH(bench_static_storage, std::string, msd::array_storage<std::string, channel_capacity>, string_input<1000>);

BENCH(bench_dynamic_storage, data, msd::queue_storage<data>, struct_input);
BENCH(bench_dynamic_storage, data, msd::vector_storage<data>, struct_input);
BENCH(bench_static_storage, data, msd::array_storage<data, channel_capacity>, struct_input);

BENCHMARK_MAIN();
