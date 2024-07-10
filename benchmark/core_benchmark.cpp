#include "iostream"

#include "benchmark/benchmark.h"
#include "app.h"
#include "ranges"

void run() {
    char *a[5] = {"text", "-source=../text.txt", "-forbidden=../forbidden.txt", "-result=../result.txt", "-min_count=10"};
    AppRunner app(4, a);
    app.run();
}

static void BM_run(benchmark::State& state) {
    for (auto _ : state) {
        run();
    }
}

BENCHMARK(BM_run);
BENCHMARK_MAIN();