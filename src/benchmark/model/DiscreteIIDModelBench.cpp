#include "benchmark/benchmark.h"

#include "DiscreteIIDModel.hpp"

using tops::model::Sequence;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;

static void BM_DiscreteIIDModelChoose(benchmark::State& state) {
  state.PauseTiming();
  std::vector<double> counts(state.range_x(), 13.0);   
  auto model = DiscreteIIDModelPtr(new DiscreteIIDModel(counts));
  state.ResumeTiming();
  while (state.KeepRunning())
    model->choose();
}
BENCHMARK(BM_DiscreteIIDModelChoose)->Range(2, 16);

static void BM_DiscreteIIDModelEvaluate(benchmark::State& state) {
  state.PauseTiming();
  std::vector<double> counts(state.range_x(), 13.0);   
  auto model = DiscreteIIDModelPtr(new DiscreteIIDModel(counts));
  state.ResumeTiming();
  while (state.KeepRunning()) {
    state.PauseTiming();
    Sequence sequence(state.range_y(), 0);
    state.ResumeTiming();
    model->evaluate(sequence, 0, state.range_y());
  }
}
BENCHMARK(BM_DiscreteIIDModelEvaluate)->RangePair(2, 16, 2, 5*1024*1024*1024);