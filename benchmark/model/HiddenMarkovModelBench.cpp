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

#include "model/HiddenMarkovModel.hpp"
#include "model/Sequence.hpp"

#include "helper/HiddenMarkovModel.hpp"
#include "helper/Sequence.hpp"

using tops::model::HiddenMarkovModel;
using tops::model::HiddenMarkovModelPtr;
using tops::model::Sequence;

using tops::helper::generateRandomHMM;
using tops::helper::generateRandomSequence;

static void BM_HiddenMarkovModelViterbi(benchmark::State& state) {
  state.PauseTiming();
  auto model = generateRandomHMM(state.range_x(), state.range_x());
  state.ResumeTiming();
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto sequence = generateRandomSequence(state.range_y(), state.range_x());
    Sequence path;
    tops::Matrix gamma;
    state.ResumeTiming();
    model->viterbi(sequence, path, gamma);
  }
}
BENCHMARK(BM_HiddenMarkovModelViterbi)->RangePair(2, 16, 2, 1024*1024*1024);

static void BM_HiddenMarkovModelForward(benchmark::State& state) {
  state.PauseTiming();
  auto model = generateRandomHMM(state.range_x(), state.range_x());
  state.ResumeTiming();
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto sequence = generateRandomSequence(state.range_y(), state.range_x());
    tops::Matrix alpha;
    state.ResumeTiming();
    model->forward(sequence, alpha);
  }
}
BENCHMARK(BM_HiddenMarkovModelForward)->RangePair(2, 16, 2, 1024*1024*1024);

static void BM_HiddenMarkovModelBackward(benchmark::State& state) {
  state.PauseTiming();
  auto model = generateRandomHMM(state.range_x(), state.range_x());
  state.ResumeTiming();
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto sequence = generateRandomSequence(state.range_y(), state.range_x());
    tops::Matrix beta;
    state.ResumeTiming();
    model->backward(sequence, beta);
  }
}
BENCHMARK(BM_HiddenMarkovModelBackward)->RangePair(2, 16, 2, 1024*1024*1024);

static void BM_HiddenMarkovModelPosteriorDecoding(benchmark::State& state) {
  state.PauseTiming();
  auto model = generateRandomHMM(state.range_x(), state.range_x());
  state.ResumeTiming();
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto sequence = generateRandomSequence(state.range_y(), state.range_x());
    tops::Matrix m;
    Sequence path;
    state.ResumeTiming();
    model->posteriorDecoding(sequence, path, m);
  }
}
BENCHMARK(BM_HiddenMarkovModelPosteriorDecoding)->RangePair(2, 16, 2, 1024*1024*1024);
