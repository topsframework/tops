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

using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;
using tops::model::Sequence;

static void BM_DiscreteIIDModelChoose(benchmark::State& state) {
  state.PauseTiming();
  std::vector<double> counts(state.range_x(), 13.0);
  auto model = DiscreteIIDModel::make(counts);
  state.ResumeTiming();
  while (state.KeepRunning())
    model->choose();
}
BENCHMARK(BM_DiscreteIIDModelChoose)->Range(2, 16);

static void BM_DiscreteIIDModelEvaluate(benchmark::State& state) {
  state.PauseTiming();
  std::vector<double> counts(state.range_x(), 13.0);
  auto model = DiscreteIIDModel::make(counts);
  state.ResumeTiming();
  while (state.KeepRunning()) {
    state.PauseTiming();
    Sequence sequence(state.range_y(), 0);
    state.ResumeTiming();
    model->evaluate(sequence, 0, state.range_y());
  }
}
BENCHMARK(BM_DiscreteIIDModelEvaluate)->RangePair(2, 16, 2, 5*1024*1024*1024);
