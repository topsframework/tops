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

// Standard headers
#include <vector>
#include <utility>

// External headers
#include "benchmark/benchmark.h"

// ToPS headers
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
  auto model = generateRandomIIDModel(state.range_x());
  while (state.KeepRunning())
    model->standardGenerator()->drawSequence(10000);
}

BENCHMARK(BM_DiscreteIIDModelChoose)->Range(32, 512);


static void BM_DiscreteIIDModelEvaluate(benchmark::State& state) {
  auto model = generateRandomIIDModel(state.range_x());
  auto sequence = generateRandomSequence(state.range_y(), state.range_x());
  auto evaluator = model->standardEvaluator(sequence);
  while (state.KeepRunning())
    evaluator->evaluateSequence(0, state.range_y());
}

BENCHMARK(BM_DiscreteIIDModelEvaluate)->RangePair(
  2, 16, 1024, 16 * 1024 * 1024);


static void BM_DiscreteIIDModelEvaluateWithCache(benchmark::State& state) {
  auto model = generateRandomIIDModel(state.range_x());
  auto sequence = generateRandomSequence(state.range_y(), state.range_x());
  auto evaluator = model->standardEvaluator(sequence, true);
  // it just initialize the cache
  evaluator->evaluateSequence(0, state.range_y());
  while (state.KeepRunning())
    evaluator->evaluateSequence(0, state.range_y());
}

BENCHMARK(BM_DiscreteIIDModelEvaluateWithCache)->RangePair(
  2, 16, 1024, 16 * 1024 * 1024);

static void BM_DiscreteIIDModelTrainML(benchmark::State& state) {
  std::vector<Sequence> training_set;
  for (int i = 0; i < state.range_y(); i++)
    training_set.push_back(generateRandomSequence(8*1024, state.range_x()));
  auto iid_trainer = DiscreteIIDModel::standardTrainer();
  iid_trainer->add_training_set(std::move(training_set));
  while (state.KeepRunning())
    iid_trainer->train(
      DiscreteIIDModel::maximum_likehood_algorithm{}, state.range_x());
}

BENCHMARK(BM_DiscreteIIDModelTrainML)->RangePair(2, 16, 512, 8 * 1024);

static void BM_DiscreteIIDModelTrainSmoothedHistogramBurgeAlgorithm(
    benchmark::State& state) {
  std::vector<Sequence> training_set;
  training_set.push_back(generateRandomSequence(state.range_y(), 256));
  auto iid_trainer = DiscreteIIDModel::standardTrainer();
  iid_trainer->add_training_set(std::move(training_set));
  while (state.KeepRunning())
    iid_trainer->train(
      DiscreteIIDModel::smoothed_histogram_burge_algorithm{}, 1.0, 15000);
}

BENCHMARK(BM_DiscreteIIDModelTrainSmoothedHistogramBurgeAlgorithm)->RangePair(
  2, 16, 512, 8 * 1024);

static void BM_DiscreteIIDModelTrainSmoothedHistogramStankeAlgorithm(
    benchmark::State& state) {
  std::vector<Sequence> training_set;
  training_set.push_back(generateRandomSequence(state.range_y(), 256));
  auto iid_trainer = DiscreteIIDModel::standardTrainer();
  iid_trainer->add_training_set(std::move(training_set));
  while (state.KeepRunning())
    iid_trainer->train(
      DiscreteIIDModel::smoothed_histogram_stanke_algorithm{},
      std::vector<unsigned int>{1}, 15000, 8, 0.5);
}

BENCHMARK(BM_DiscreteIIDModelTrainSmoothedHistogramStankeAlgorithm)->RangePair(
  2, 16, 512, 8 * 1024);

static void BM_DiscreteIIDModelTrainSmoothedHistogramKernelDensityAlgorithm(
    benchmark::State& state) {
  std::vector<Sequence> training_set;
  training_set.push_back(generateRandomSequence(state.range_y(), 256));
  auto iid_trainer = DiscreteIIDModel::standardTrainer();
  iid_trainer->add_training_set(std::move(training_set));
  while (state.KeepRunning())
    iid_trainer->train(
      DiscreteIIDModel::smoothed_histogram_kernel_density_algorithm{}, 15000);
}

BENCHMARK(
  BM_DiscreteIIDModelTrainSmoothedHistogramKernelDensityAlgorithm)->RangePair(
    2, 16, 512, 8 * 1024);
