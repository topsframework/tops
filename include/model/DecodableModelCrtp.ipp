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

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

template<typename Derived>
DecodableModelCrtp<Derived>::DecodableModelCrtp(
    std::vector<StatePtr> states,
    DiscreteIIDModelPtr initial_probabilities,
    unsigned int state_alphabet_size,
    unsigned int observation_alphabet_size)
    : _states(std::move(states)),
      _initial_probabilities(initial_probabilities),
      _state_alphabet_size(state_alphabet_size),
      _observation_alphabet_size(observation_alphabet_size) {
}

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

/*================================  TRAINER  =================================*/

template<typename Derived>
TrainerPtr<Labeling, Derived>
DecodableModelCrtp<Derived>::labelingTrainer() {
  return SimpleTrainer<Labeling, Derived>::make();
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
TrainerPtr<Labeling, Derived>
DecodableModelCrtp<Derived>::labelingTrainer(DerivedPtr model) {
  return FixedTrainer<Labeling, Derived>::make(model);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
template<typename Tag, typename... Args>
TrainerPtr<Labeling, Derived>
DecodableModelCrtp<Derived>::labelingTrainer(Tag, Args&&... args) {
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
    const Standard<Sequence>& sequence, bool cached) {
  if (cached)
    return CachedLabeler<Derived>::make(make_shared(), sequence);
  return SimpleLabeler<Derived>::make(make_shared(), sequence);
}

/*==============================  CALCULATOR  ================================*/

template<typename Derived>
CalculatorPtr DecodableModelCrtp<Derived>::calculator(
    const Standard<Sequence>& sequence, bool cached) {
  if (cached)
    return CachedCalculator<Derived>::make(make_shared(), sequence);
  return SimpleCalculator<Derived>::make(make_shared(), sequence);
}

/*----------------------------------------------------------------------------*/
/*                              VIRTUAL METHODS                               */
/*----------------------------------------------------------------------------*/

template<typename Derived>
unsigned int DecodableModelCrtp<Derived>::stateAlphabetSize() const {
  return _state_alphabet_size;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
unsigned int DecodableModelCrtp<Derived>::observationAlphabetSize() const {
  return _observation_alphabet_size;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
auto DecodableModelCrtp<Derived>::state(unsigned int id) -> StatePtr {
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
auto DecodableModelCrtp<Derived>::make_shared() -> DerivedPtr {
  return std::static_pointer_cast<Derived>(
    static_cast<Derived*>(this)->shared_from_this());
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
