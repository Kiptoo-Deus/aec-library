#include <benchmark/benchmark.h>
#include "aec/aec.hpp"
#include <random>
#include <vector>

static void BM_AEC_Process_FixedPoint(benchmark::State& state) {
    aec::AECConfig config;
    config.use_fixed_point = true;
    config.frame_size = static_cast<uint32_t>(state.range(0));
    
    auto aec = aec::create_aec(config);
    
    std::vector<int16_t> far_end(config.frame_size);
    std::vector<int16_t> near_end(config.frame_size);
    std::vector<int16_t> output(config.frame_size);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int16_t> dis(-1000, 1000);
    
    for (auto& sample : far_end) sample = dis(gen);
    for (auto& sample : near_end) sample = dis(gen);
    
    for (auto _ : state) {
        aec->process(far_end.data(), near_end.data(), output.data(), config.frame_size);
        benchmark::DoNotOptimize(output);
    }
    
    state.SetItemsProcessed(state.iterations() * config.frame_size);
}

BENCHMARK(BM_AEC_Process_FixedPoint)
    ->RangeMultiplier(2)->Range(64, 4096)
    ->Unit(benchmark::kMicrosecond);

static void BM_AEC_Latency(benchmark::State& state) {
    aec::AECConfig config;
    config.use_fixed_point = true;
    config.frame_size = 256;
    
    auto aec = aec::create_aec(config);
    
    std::vector<int16_t> far_end(config.frame_size);
    std::vector<int16_t> near_end(config.frame_size);
    std::vector<int16_t> output(config.frame_size);
    
    for (auto _ : state) {
        aec->process(far_end.data(), near_end.data(), output.data(), config.frame_size);
        double latency = aec->get_latency_ms();
        benchmark::DoNotOptimize(latency);
        state.PauseTiming();
        aec->reset();
        state.ResumeTiming();
    }
}

BENCHMARK(BM_AEC_Latency)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
