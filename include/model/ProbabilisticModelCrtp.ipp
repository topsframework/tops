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
TrainerPtr<Standard, Derived>
ProbabilisticModelCrtp<Derived>::standardTrainer() {
  return SimpleTrainer<Standard, Derived>::make();
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
TrainerPtr<Standard, Derived>
ProbabilisticModelCrtp<Derived>::standardTrainer(DerivedPtr model) {
  return FixedTrainer<Standard, Derived>::make(model);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
template<typename Tag, typename... Args>
TrainerPtr<Standard, Derived>
ProbabilisticModelCrtp<Derived>::standardTrainer(Tag, Args&&... args) {
  return CachedTrainer<Standard, Derived, Tag, Args...>::make(
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
EvaluatorPtr<Standard> ProbabilisticModelCrtp<Derived>::standardEvaluator(
    const Standard<Sequence>& sequence, bool cached) {
  auto self_ptr = make_shared();
  return cached ? CachedEvaluator<Standard, Derived>::make(self_ptr, sequence)
                : SimpleEvaluator<Standard, Derived>::make(self_ptr, sequence);
}

/*===============================  GENERATOR  ================================*/

template<typename Derived>
GeneratorPtr<Standard> ProbabilisticModelCrtp<Derived>::standardGenerator(
    RandomNumberGeneratorPtr rng) {
  return SimpleGenerator<Standard, Derived>::make(make_shared(), rng);
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
void ProbabilisticModelCrtp<Derived>::initializeCache(CEPtr<Standard> evaluator,
                                                      unsigned int phase) {
  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;
  prefix_sum_array.resize(evaluator->sequence().size() + 1);

  prefix_sum_array[0] = 1;
  for (unsigned int i = 0; i < evaluator->sequence().size(); i++)
    prefix_sum_array[i+1]
      = prefix_sum_array[i] * evaluator->evaluateSymbol(i, phase);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
Probability ProbabilisticModelCrtp<Derived>::evaluateSequence(
    SEPtr<Standard> evaluator,
    unsigned int begin,
    unsigned int end,
    unsigned int phase) const {
  Probability prob = 1;
  for (unsigned int i = begin; i < end; i++)
    prob *= evaluator->evaluateSymbol(i, phase);
  return prob;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
Probability ProbabilisticModelCrtp<Derived>::evaluateSymbol(
    CEPtr<Standard> evaluator,
    unsigned int pos,
    unsigned int phase) const {
  return evaluateSymbol(static_cast<SEPtr<Standard>>(evaluator), pos, phase);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
Probability ProbabilisticModelCrtp<Derived>::evaluateSequence(
    CEPtr<Standard> evaluator,
    unsigned int begin,
    unsigned int end,
    unsigned int /* phase */) const {
  auto& prefix_sum_array = evaluator->cache().prefix_sum_array;
  return prefix_sum_array[end] / prefix_sum_array[begin];
}

/*===============================  GENERATOR  ================================*/

template<typename Derived>
Standard<Sequence> ProbabilisticModelCrtp<Derived>::drawSequence(
    SGPtr<Standard> generator,
    unsigned int size,
    unsigned int phase) const {
  Sequence s;
  for (unsigned int k = 0; k < size; k++)
      s.push_back(generator->drawSymbol(k, phase, s));
  return s;
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
