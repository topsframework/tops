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

#include "benchmark/benchmark.h"

#include "model/DiscreteIIDModel.hpp"
#include "model/Sequence.hpp"

#include "helper/DiscreteIIDModel.hpp"
#include "helper/Sequence.hpp"

using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;
using tops::model::Sequence;

using tops::helper::generateRandomIIDModel;
using tops::helper::generateRandomSequence;

static void BM_DiscreteIIDModelChoose(benchmark::State& state) {
  state.PauseTiming();
  auto model = generateRandomIIDModel(state.range_x());
  state.ResumeTiming();
  while (state.KeepRunning())
    model->draw();
}
BENCHMARK(BM_DiscreteIIDModelChoose)->Range(2, 16);

static void BM_DiscreteIIDModelEvaluate(benchmark::State& state) {
  state.PauseTiming();
  auto model = generateRandomIIDModel(state.range_x());
  state.ResumeTiming();
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto sequence = generateRandomSequence(state.range_y(), state.range_x());
    state.ResumeTiming();
    model->standardEvaluator(sequence, 0, state.range_y()-1);
  }
}
BENCHMARK(BM_DiscreteIIDModelEvaluate)->RangePair(2, 16, 2, 1024*1024*1024);
