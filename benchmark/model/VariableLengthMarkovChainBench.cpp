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

#include "model/VariableLengthMarkovChain.hpp"
#include "model/Sequence.hpp"
#include "model/VariableLengthMarkovChain.hpp"

#include "helper/Random.hpp"
#include "helper/Sequence.hpp"
#include "helper/VariableLengthMarkovChain.hpp"

using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::Sequence;

using tops::helper::generateRandomInteger;
using tops::helper::generateSequence;
using tops::helper::generateRandomVLMC;

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