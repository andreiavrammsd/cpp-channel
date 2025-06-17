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

    2025-06-17T19:55:02+03:00
    Running ./benchmarks/channel_benchmark
    Run on (8 X 4000.08 MHz CPU s)
    CPU Caches:
    L1 Data 32 KiB (x4)
    L1 Instruction 32 KiB (x4)
    L2 Unified 256 KiB (x4)
    L3 Unified 8192 KiB (x1)
    Load Average: 1.38, 1.22, 1.06
    ------------------------------------------------------------------------------------------------------------------------------------------------------------
    Benchmark                                                                                                                  Time             CPU   Iterations
    ------------------------------------------------------------------------------------------------------------------------------------------------------------
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<100000>>_mean                     652607002 ns    226690848 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<100000>>_median                   651695229 ns    225379690 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<100000>>_stddev                    12253781 ns     15462972 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<100000>>_cv                            1.88 %          6.82 %            10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<100000>>_max                      672915805 ns    255534858 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<100000>>_mean                    974087950 ns    514260828 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<100000>>_median                  977160289 ns    516344216 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<100000>>_stddev                   18312948 ns     28280400 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<100000>>_cv                           1.88 %          5.50 %            10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<100000>>_max                    1003003285 ns    558790265 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<100000>>_mean    628774895 ns    213404616 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<100000>>_median  629143659 ns    215630841 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<100000>>_stddev    8790540 ns      8340659 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<100000>>_cv           1.40 %          3.91 %            10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<100000>>_max     640584436 ns    224198673 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<1000>>_mean                        43353148 ns     33321779 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<1000>>_median                      43035735 ns     33114531 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<1000>>_stddev                        626857 ns       516438 ns           10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<1000>>_cv                              1.45 %          1.55 %            10
    bench_dynamic_storage<std::string, msd::queue_storage<std::string>, string_input<1000>>_max                         44420815 ns     34055142 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<1000>>_mean                      143175350 ns    134608661 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<1000>>_median                    143349862 ns    135104870 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<1000>>_stddev                      9874397 ns      9112605 ns           10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<1000>>_cv                             6.90 %          6.77 %            10
    bench_dynamic_storage<std::string, msd::vector_storage<std::string>, string_input<1000>>_max                       160931701 ns    149620486 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<1000>>_mean       37482750 ns     36598866 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<1000>>_median     37678000 ns     36697213 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<1000>>_stddev       972055 ns       739164 ns           10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<1000>>_cv             2.59 %          2.02 %            10
    bench_static_storage<std::string, msd::array_storage<std::string, channel_capacity>, string_input<1000>>_max        38740257 ns     37767023 ns           10
    bench_dynamic_storage<data, msd::queue_storage<data>, struct_input>_mean                                            56195102 ns     37959789 ns           10
    bench_dynamic_storage<data, msd::queue_storage<data>, struct_input>_median                                          56222959 ns     37916027 ns           10
    bench_dynamic_storage<data, msd::queue_storage<data>, struct_input>_stddev                                            239106 ns       192415 ns           10
    bench_dynamic_storage<data, msd::queue_storage<data>, struct_input>_cv                                                  0.43 %          0.51 %            10
    bench_dynamic_storage<data, msd::queue_storage<data>, struct_input>_max                                             56524553 ns     38392052 ns           10
    bench_dynamic_storage<data, msd::vector_storage<data>, struct_input>_mean                                          318745363 ns    299820882 ns           10
    bench_dynamic_storage<data, msd::vector_storage<data>, struct_input>_median                                        333031832 ns    312967363 ns           10
    bench_dynamic_storage<data, msd::vector_storage<data>, struct_input>_stddev                                         30118977 ns     28236407 ns           10
    bench_dynamic_storage<data, msd::vector_storage<data>, struct_input>_cv                                                 9.45 %          9.42 %            10
    bench_dynamic_storage<data, msd::vector_storage<data>, struct_input>_max                                           343551976 ns    323198986 ns           10
    bench_static_storage<data, msd::array_storage<data, channel_capacity>, struct_input>_mean                           39037187 ns     32142886 ns           10
    bench_static_storage<data, msd::array_storage<data, channel_capacity>, struct_input>_median                         39015373 ns     32017939 ns           10
    bench_static_storage<data, msd::array_storage<data, channel_capacity>, struct_input>_stddev                           557539 ns       701550 ns           10
    bench_static_storage<data, msd::array_storage<data, channel_capacity>, struct_input>_cv                                 1.43 %          2.18 %            10
    bench_static_storage<data, msd::array_storage<data, channel_capacity>, struct_input>_max                            40336146 ns     33191282 ns           10
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
    const auto input = Input::make();

    for (auto _ : state) {
        msd::channel<T, Storage> channel{channel_capacity};

        std::thread producer([&] {
            for (std::size_t i = 0; i < number_of_inputs; ++i) {
                channel << input;
            }
            channel.close();
        });

        for (auto& value : channel) {
            volatile auto* do_not_optimize = &value;
            (void)do_not_optimize;
        }

        producer.join();
    }
}

template <typename T, typename Storage, typename Input>
static void bench_static_storage(benchmark::State& state)
{
    const auto input = Input::make();

    for (auto _ : state) {
        msd::channel<T, Storage> channel{};

        std::thread producer([&] {
            for (std::size_t i = 0; i < number_of_inputs; ++i) {
                channel << input;
            }
            channel.close();
        });

        for (auto& value : channel) {
            volatile auto* do_not_optimize = &value;
            (void)do_not_optimize;
        }

        producer.join();
    }
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
