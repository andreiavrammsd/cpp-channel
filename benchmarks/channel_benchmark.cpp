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

    2025-06-17T18:36:15+03:00
    Running ./benchmarks/channel_benchmark
    Run on (8 X 3998.1 MHz CPU s)
    CPU Caches:
    L1 Data 32 KiB (x4)
    L1 Instruction 32 KiB (x4)
    L2 Unified 256 KiB (x4)
    L3 Unified 8192 KiB (x1)
    Load Average: 1.39, 1.23, 0.98
    ----------------------------------------------------------------------------------------------------------------------------------------------------
    Benchmark                                                                                                          Time             CPU   Iterations
    ----------------------------------------------------------------------------------------------------------------------------------------------------
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input>_mean                    1135532785 ns   1102075601 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input>_median                  1126981431 ns   1096525909 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input>_stddev                    16748064 ns     13438856 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input>_cv                            1.47 %          1.22 %            10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input>_mean                   1349502195 ns   1312254745 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input>_median                 1343862530 ns   1309941538 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input>_stddev                   24872877 ns     17496752 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input>_cv                           1.84 %          1.33 %            10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input>_mean   1116143998 ns   1083327478 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input>_median 1100725718 ns   1069640108 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input>_stddev   27816556 ns     24827916 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input>_cv           2.49 %          2.29 %            10
    bench_dynamic_storage<data, msd::queue_storage<data>, data_input>_mean                                           110 ns          106 ns           10
    bench_dynamic_storage<data, msd::queue_storage<data>, data_input>_median                                         110 ns          106 ns           10
    bench_dynamic_storage<data, msd::queue_storage<data>, data_input>_stddev                                       0.279 ns        0.298 ns           10
    bench_dynamic_storage<data, msd::queue_storage<data>, data_input>_cv                                            0.25 %          0.28 %            10
    bench_dynamic_storage<data, msd::vector_storage<data>, data_input>_mean                                          274 ns          266 ns           10
    bench_dynamic_storage<data, msd::vector_storage<data>, data_input>_median                                        276 ns          267 ns           10
    bench_dynamic_storage<data, msd::vector_storage<data>, data_input>_stddev                                       11.2 ns         10.9 ns           10
    bench_dynamic_storage<data, msd::vector_storage<data>, data_input>_cv                                           4.07 %          4.11 %            10
    bench_static_storage<data, msd::array_storage<data, channel_capacity>, data_input>_mean                          104 ns          102 ns           10
    bench_static_storage<data, msd::array_storage<data, channel_capacity>, data_input>_median                        104 ns          102 ns           10
    bench_static_storage<data, msd::array_storage<data, channel_capacity>, data_input>_stddev                      0.619 ns        0.304 ns           10
    bench_static_storage<data, msd::array_storage<data, channel_capacity>, data_input>_cv                           0.60 %          0.30 %            10
 */
// clang-format on

static constexpr std::size_t channel_capacity = 1024;
static constexpr std::size_t number_of_inputs = 100000;

struct data {
    std::array<int, 1000> data{};
};

struct data_input {
    static data make() { return data{}; }
};

struct string_input {
    static std::string make() { return std::string(100000, 'c'); }
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
        for (auto value : channel) {
            benchmark::DoNotOptimize(value);
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
        for (auto value : channel) {
            benchmark::DoNotOptimize(value);
        }
    }

    producer.join();
}

BENCHMARK_TEMPLATE(bench_dynamic_storage, std::string, msd::queue_storage<std::string>, string_input);
BENCHMARK_TEMPLATE(bench_dynamic_storage, std::string, msd::vector_storage<std::string>, string_input);
BENCHMARK_TEMPLATE(bench_static_storage, std::string, msd::array_storage<std::string, channel_capacity>, string_input);
BENCHMARK_TEMPLATE(bench_dynamic_storage, data, msd::queue_storage<data>, data_input);
BENCHMARK_TEMPLATE(bench_dynamic_storage, data, msd::vector_storage<data>, data_input);
BENCHMARK_TEMPLATE(bench_static_storage, data, msd::array_storage<data, channel_capacity>, data_input);

BENCHMARK_MAIN();
