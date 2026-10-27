#include <benchmark/benchmark.h>

#include <vector>

static void bm_vector_push_back(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v;
        v.reserve(static_cast<std::size_t>(state.range(0)));
        for (int i = 0; i < state.range(0); ++i) {
            v.push_back(i);
        }
        benchmark::DoNotOptimize(v.data());
        benchmark::ClobberMemory();
    }
}
BENCHMARK(bm_vector_push_back)->Range(8, 8 << 10);