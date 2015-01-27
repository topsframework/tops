/***********************************************************************/
/*  Copyright 2015 ToPS                                                */
/*                                                                     */
/*  This program is free software; you can redistribute it and/or      */
/*  modify it under the terms of the GNU  General Public License as    */
/*  published by the Free Software Foundation; either version 3 of     */
/*  the License, or (at your option) any later version.                */
/*                                                                     */
/*  This program is distributed in the hope that it will be useful,    */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of     */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      */
/*  GNU General Public License for more details.                       */
/*                                                                     */
/*  You should have received a copy of the GNU General Public License  */
/*  along with this program; if not, write to the Free Software        */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,         */
/*  MA 02110-1301, USA.                                                */
/***********************************************************************/

#include <vector>
#include <random>
#include <string>

#include "benchmark/benchmark.h"

#include "VariableLengthMarkovChain.hpp"
#include "Sequence.hpp"
#include "VariableLengthMarkovChain.hpp"

using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::Sequence;

std::default_random_engine generator;

int generateRandomInteger(int max) {
  std::uniform_int_distribution<int> distribution(0, max);
  return distribution(generator);
}

Sequence generateSequence(int size, int alphabet_size) {
  Sequence sequence;
  for (int i = 0; i < size; i++) {
    sequence.push_back(generateRandomInteger(alphabet_size-1));
  }
  return sequence;
}

VariableLengthMarkovChainPtr generateRandomVLMC(int number_of_nodes, int alphabet_size) {
  auto alphabet = tops::AlphabetPtr(new tops::Alphabet());
  for (int i = 0; i < alphabet_size; i++)
    alphabet->createSymbol(std::to_string(i));
  std::vector<double> counts(alphabet_size, 13.0);
  auto iid = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(counts));
  auto tree = tops::ContextTreePtr(new tops::ContextTree(alphabet));
  auto node = tree->createContext();
  node->setDistribution(iid);
  for (int i = 1; i < number_of_nodes; i++) {
    auto _node = tree->createContext();
    _node->setDistribution(iid);
    node->setChild(_node, generateRandomInteger(alphabet_size-1));
    node = _node;
  }
  return VariableLengthMarkovChain::make(tree);
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