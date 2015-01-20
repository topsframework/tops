#include "benchmark/benchmark.h"

#include <iostream>
#include <random>

#include "VariableLengthMarkovChain.hpp"

using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::ContextTree;
using tops::model::ContextTreePtr;
using tops::model::ContextTreeNode;
using tops::model::ContextTreeNodePtr;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;
using tops::model::Sequence;

std::default_random_engine generator;

int generateRandomInteger(int max) {
  std::uniform_int_distribution<int> distribution(0, max);
  return distribution(generator);
}

Sequence generateSequence(int size, int alphabet_size) {
  Sequence sequence;
  for (int i = 0; i < size; i ++) {
    sequence.push_back(generateRandomInteger(alphabet_size-1));
  }
  return sequence;
}

VariableLengthMarkovChainPtr generateRandomVLMC(int number_of_nodes, int alphabet_size) {
  std::vector<double> counts(alphabet_size, 13.0);
  auto iid = DiscreteIIDModelPtr(new DiscreteIIDModel(counts));
  auto tree = ContextTreePtr(new ContextTree(alphabet_size));
  auto node = tree->createContext(iid);
  for (int i = 1; i < number_of_nodes; i++) {
    auto _node = tree->createContext(iid);
    node->child(_node, generateRandomInteger(alphabet_size-1));
    node = _node;
  }
  return VariableLengthMarkovChainPtr(new VariableLengthMarkovChain(tree, 2));
}

static void BM_VariableLengthMarkovChainChoose(benchmark::State& state) {
  state.PauseTiming();
  auto model = generateRandomVLMC(state.range_x(), 2);
  auto sequence = generateSequence(state.range_y(), 2);
  int index = generateRandomInteger(state.range_y()-1);
  state.ResumeTiming();
  while (state.KeepRunning())
    model->choosePosition(sequence, index);
}
BENCHMARK(BM_VariableLengthMarkovChainChoose)->RangePair(2, 16, 2, 5*1024*1024*1024);

static void BM_VariableLengthMarkovChainEvaluate(benchmark::State& state) {
  state.PauseTiming();
  auto model = generateRandomVLMC(state.range_x(), 2);
  auto sequence = generateSequence(state.range_y(), 2);
  state.ResumeTiming();
  while (state.KeepRunning()) {
    model->evaluate(sequence, 0, state.range_y());
  }
}
BENCHMARK(BM_VariableLengthMarkovChainEvaluate)->RangePair(2, 16, 2, 5*1024*1024*1024);
