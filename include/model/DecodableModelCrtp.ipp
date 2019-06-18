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
#include <memory>
#include <vector>
#include <utility>

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

template<typename Derived>
DecodableModelCrtp<Derived>::DecodableModelCrtp(
    std::vector<StatePtr> states,
    size_t state_alphabet_size,
    size_t observation_alphabet_size)
    : _states(std::move(states)),
      _state_alphabet_size(state_alphabet_size),
      _observation_alphabet_size(observation_alphabet_size) {
}

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

/*================================  TRAINER  =================================*/

template<typename Derived>
TrainerPtr<Multiple, Derived>
DecodableModelCrtp<Derived>::unsupervisedTrainer() {
  return SimpleTrainer<Multiple, Derived>::make();
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
TrainerPtr<Multiple, Derived>
DecodableModelCrtp<Derived>::unsupervisedTrainer(DerivedPtr model) {
  return FixedTrainer<Multiple, Derived>::make(model);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
template<typename Tag, typename... Args>
TrainerPtr<Multiple, Derived>
DecodableModelCrtp<Derived>::unsupervisedTrainer(Tag, Args&&... args) {
  return CachedTrainer<Multiple, Derived, Tag, Args...>::make(
    Tag{}, std::forward<Args>(args)...);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
TrainerPtr<Labeling, Derived>
DecodableModelCrtp<Derived>::supervisedTrainer() {
  return SimpleTrainer<Labeling, Derived>::make();
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
TrainerPtr<Labeling, Derived>
DecodableModelCrtp<Derived>::supervisedTrainer(DerivedPtr model) {
  return FixedTrainer<Labeling, Derived>::make(model);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
template<typename Tag, typename... Args>
TrainerPtr<Labeling, Derived>
DecodableModelCrtp<Derived>::supervisedTrainer(Tag, Args&&... args) {
  return CachedTrainer<Labeling, Derived, Tag, Args...>::make(
    Tag{}, std::forward<Args>(args)...);
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

template<typename Derived>
EvaluatorPtr<Labeling> DecodableModelCrtp<Derived>::labelingEvaluator(
    const Labeling<Sequence>& sequence, bool cached) {
  if (cached)
    return CachedEvaluator<Labeling, Derived>::make(make_shared(), sequence);
  return SimpleEvaluator<Labeling, Derived>::make(make_shared(), sequence);
}

/*===============================  GENERATOR  ================================*/

template<typename Derived>
GeneratorPtr<Labeling> DecodableModelCrtp<Derived>::labelingGenerator(
    RandomNumberGeneratorPtr rng) {
  return SimpleGenerator<Labeling, Derived>::make(make_shared(), rng);
}

/*================================  LABELER  =================================*/

template<typename Derived>
LabelerPtr DecodableModelCrtp<Derived>::labeler(
    const Multiple<Sequence>& sequence, bool cached) {
  return cached ? CachedLabeler<Derived>::make(make_shared(), sequence)
                : SimpleLabeler<Derived>::make(make_shared(), sequence);
}

/*==============================  CALCULATOR  ================================*/

template<typename Derived>
CalculatorPtr DecodableModelCrtp<Derived>::calculator(
    const Multiple<Sequence>& sequence, bool cached) {
  return cached ? CachedCalculator<Derived>::make(make_shared(), sequence)
                : SimpleCalculator<Derived>::make(make_shared(), sequence);
}

/*----------------------------------------------------------------------------*/
/*                           PURELY VIRTUAL METHODS                           */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

template<typename Derived>
Probability DecodableModelCrtp<Derived>::evaluateSequence(
    SEPtr<Labeling> evaluator,
    size_t begin,
    size_t end,
    size_t phase) const {
  Probability prob = 1;
  for (size_t i = begin; i < end; i++)
    prob *= evaluator->evaluateSymbol(i, phase);
  return prob;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
void DecodableModelCrtp<Derived>::initializeCache(CEPtr<Labeling> evaluator,
                                                  size_t phase) {
  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;
  prefix_sum_array.resize(evaluator->sequence().observations[0].size() + 1);

  prefix_sum_array[0] = 1;
  for (size_t i = 0; i < evaluator->sequence().observations[0].size(); i++)
    prefix_sum_array[i+1]
      = prefix_sum_array[i] * evaluator->evaluateSymbol(i, phase);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
Probability DecodableModelCrtp<Derived>::evaluateSymbol(
    CEPtr<Labeling> evaluator,
    size_t pos,
    size_t phase) const {
  return evaluateSymbol(static_cast<SEPtr<Labeling>>(evaluator), pos, phase);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
Probability DecodableModelCrtp<Derived>::evaluateSequence(
    CEPtr<Labeling> evaluator,
    size_t begin,
    size_t end,
    size_t /* phase */) const {
  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;
  return prefix_sum_array[end] / prefix_sum_array[begin];
}

/*===============================  GENERATOR  ================================*/

template<typename Derived>
Labeling<Sequence> DecodableModelCrtp<Derived>::drawSequence(
    SGPtr<Labeling> generator,
    size_t size,
    size_t phase) const {
  Labeling<Sequence> labeling;
  labeling.observations.resize(1);

  labeling.label.push_back(_begin_id);
  for (size_t i = 1; i <= size; i++) {
    auto[ symbols, label ]
      = generator->drawSymbol(i, phase, labeling);

    // Keep trying to emit the right number of symbols
    if (label == _end_id) { i--; continue; }

    labeling.label.push_back(label);

    for (size_t j = 0; j < labeling.observations.size(); j++)
      labeling.observations[i].push_back(symbols[j]);
  }
  labeling.label.push_back(_end_id);

  return labeling;
}

/*----------------------------------------------------------------------------*/
/*                              VIRTUAL METHODS                               */
/*----------------------------------------------------------------------------*/

template<typename Derived>
size_t DecodableModelCrtp<Derived>::stateAlphabetSize() const {
  return _state_alphabet_size;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
size_t DecodableModelCrtp<Derived>::observationAlphabetSize() const {
  return _observation_alphabet_size;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
auto DecodableModelCrtp<Derived>::state(typename State::Id id) -> StatePtr {
  return _states[id];
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
auto DecodableModelCrtp<Derived>::states() -> std::vector<StatePtr> {
  return _states;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
auto DecodableModelCrtp<Derived>::states() const
    -> const std::vector<StatePtr> {
  return _states;
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename Derived>
std::shared_ptr<Derived> DecodableModelCrtp<Derived>::make_shared() {
  return std::static_pointer_cast<Derived>(
    static_cast<Derived*>(this)->shared_from_this());
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
