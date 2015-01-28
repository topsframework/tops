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
#include "InhomogeneousMarkovChain.hpp"

#include "Random.hpp"
#include "VLMC.hpp"

using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::InhomogeneousMarkovChain;
using tops::model::InhomogeneousMarkovChainPtr;
using tops::model::Sequence;

using tops::helper::generateRandomInteger;
using tops::helper::generateSequence;
using tops::helper::generateRandomVLMC;

InhomogeneousMarkovChainPtr generateRandomIMC(int number_of_chains, int alphabet_size) {
  std::vector<VariableLengthMarkovChainPtr> vlmcs;
  for (int i = 0; i < number_of_chains; i++)
    vlmcs.push_back(generateRandomVLMC(number_of_chains, alphabet_size));
  return InhomogeneousMarkovChain::make(vlmcs, true);
}

static void BM_InhomogeneousMarkovChainEvaluate(benchmark::State& state) {
  state.PauseTiming();
  auto model = generateRandomIMC(state.range_x(), 2);
  auto sequence = generateSequence(state.range_y(), 2);
  state.ResumeTiming();
  while (state.KeepRunning()) {
    model->evaluate(sequence, 0, state.range_y()-1);
  }
}
BENCHMARK(BM_InhomogeneousMarkovChainEvaluate)->RangePair(2, 16, 2, 5*1024*1024*1024);
