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
#include <utility>

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

/*================================  TRAINER  =================================*/

template<typename Derived>
TrainerPtr<Multiple, Derived>
ProbabilisticModelCrtp<Derived>::standardTrainer() {
  return SimpleTrainer<Multiple, Derived>::make();
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
TrainerPtr<Multiple, Derived>
ProbabilisticModelCrtp<Derived>::standardTrainer(DerivedPtr model) {
  return FixedTrainer<Multiple, Derived>::make(model);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
template<typename Tag, typename... Args>
TrainerPtr<Multiple, Derived>
ProbabilisticModelCrtp<Derived>::standardTrainer(Tag, Args&&... args) {
  return CachedTrainer<Multiple, Derived, Tag, Args...>::make(
    Tag{}, std::forward<Args>(args)...);
}

/*=================================  OTHERS  =================================*/

template<typename Derived>
template<typename... Args>
auto ProbabilisticModelCrtp<Derived>::make(Args&&... args)
    -> typename ProbabilisticModelCrtp<Derived>::DerivedPtr {
  return DerivedPtr(new Derived(std::forward<Args>(args)...));
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

template<typename Derived>
EvaluatorPtr<Multiple> ProbabilisticModelCrtp<Derived>::standardEvaluator(
    const Multiple<Sequence>& sequence, bool cached) {
  auto self_ptr = make_shared();
  return cached ? CachedEvaluator<Multiple, Derived>::make(self_ptr, sequence)
                : SimpleEvaluator<Multiple, Derived>::make(self_ptr, sequence);
}

/*===============================  GENERATOR  ================================*/

template<typename Derived>
GeneratorPtr<Multiple> ProbabilisticModelCrtp<Derived>::standardGenerator(
    RandomNumberGeneratorPtr rng) {
  return SimpleGenerator<Multiple, Derived>::make(make_shared(), rng);
}

/*===============================  SERIALIZER  ===============================*/

template<typename Derived>
SerializerPtr ProbabilisticModelCrtp<Derived>::serializer(
    TranslatorPtr translator) {
  return SimpleSerializer<Derived>::make(make_shared(), translator);
}

/*----------------------------------------------------------------------------*/
/*                           PURELY VIRTUAL METHODS                           */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

template<typename Derived>
void ProbabilisticModelCrtp<Derived>::initializeCache(CEPtr<Multiple> evaluator,
                                                      size_t phase) {
  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;
  prefix_sum_array.resize(evaluator->sequence()[0].size() + 1);

  prefix_sum_array[0] = 1;
  for (size_t i = 0; i < evaluator->sequence()[0].size(); i++)
    prefix_sum_array[i+1]
      = prefix_sum_array[i] * evaluator->evaluateSymbol(i, phase);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
Probability ProbabilisticModelCrtp<Derived>::evaluateSequence(
    SEPtr<Multiple> evaluator,
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
Probability ProbabilisticModelCrtp<Derived>::evaluateSymbol(
    CEPtr<Multiple> evaluator,
    size_t pos,
    size_t phase) const {
  return evaluateSymbol(static_cast<SEPtr<Multiple>>(evaluator), pos, phase);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
Probability ProbabilisticModelCrtp<Derived>::evaluateSequence(
    CEPtr<Multiple> evaluator,
    size_t begin,
    size_t end,
    size_t /* phase */) const {
  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;
  return prefix_sum_array[end] / prefix_sum_array[begin];
}

/*===============================  GENERATOR  ================================*/

template<typename Derived>
Multiple<Sequence> ProbabilisticModelCrtp<Derived>::drawSequence(
    SGPtr<Multiple> generator,
    size_t size,
    size_t phase) const {
  Multiple<Sequence> sequences(1);

  for (size_t k = 0; k < size; k++) {
    auto symbols = generator->drawSymbol(k, phase, sequences);
    for (size_t i = 0; i < symbols.size(); i++) {
      sequences[i].push_back(symbols[i]);
    }
  }

  return sequences;
}

/*===============================  SERIALIZER  ===============================*/

template<typename Derived>
void ProbabilisticModelCrtp<Derived>::serialize(SSPtr serializer) {
  serializer->translator()->translate(this->make_shared());
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename Derived>
std::shared_ptr<Derived> ProbabilisticModelCrtp<Derived>::make_shared() {
  return std::static_pointer_cast<Derived>(
    static_cast<Derived*>(this)->shared_from_this());
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
