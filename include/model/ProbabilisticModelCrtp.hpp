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

#ifndef TOPS_MODEL_PROBABILISTIC_MODEL_CRTP_
#define TOPS_MODEL_PROBABILISTIC_MODEL_CRTP_

// Standard headers
#include <memory>
#include <vector>

// Internal headers
#include "model/Trainer.hpp"
#include "model/Sequence.hpp"
#include "model/FixedTrainer.hpp"
#include "model/SimpleTrainer.hpp"
#include "model/CachedTrainer.hpp"
#include "model/SimpleGenerator.hpp"
#include "model/SimpleEvaluator.hpp"
#include "model/CachedEvaluator.hpp"
#include "model/SimpleSerializer.hpp"
#include "model/ProbabilisticModel.hpp"

namespace tops {
namespace model {

// Forward declaration
template<typename Derived>
class ProbabilisticModelCrtp;

/**
 * @typedef ProbabilisticModelCrtpPtr
 * @brief Alias of pointer to ProbabilisticModelCrtp.
 */
template<typename Derived>
using ProbabilisticModelPtrCrtp
  = std::shared_ptr<ProbabilisticModelCrtp<Derived>>;

/**
 * @class ProbabilisticModelCrtp
 * @brief Implementation of front-end with CRTP to inject methods in  subclasses
 */
template<typename Derived>
class ProbabilisticModelCrtp
    : public std::enable_shared_from_this<ProbabilisticModelCrtp<Derived>>,
      public virtual ProbabilisticModel {
 public:
  // Inner classes
  struct Cache {
    std::vector<LogProbability> prefix_sum_array;
  };

  // Alias
  using Base = void;
  using DerivedPtr = std::shared_ptr<Derived>;

  template<template<typename Target> class Decorator>
  using SEPtr = SimpleEvaluatorPtr<Decorator, Derived>;

  template<template<typename Target> class Decorator>
  using CEPtr = CachedEvaluatorPtr<Decorator, Derived>;

  template<template<typename Target> class Decorator>
  using SGPtr = SimpleGeneratorPtr<Decorator, Derived>;

  using SSPtr = SimpleSerializerPtr<Derived>;

  // Static methods
  template<typename... Args>
  static DerivedPtr make(Args&&... args);

  static TrainerPtr<Standard, Derived> standardTrainer();

  static TrainerPtr<Standard, Derived> standardTrainer(DerivedPtr model);

  template<typename Tag, typename... Args>
  static TrainerPtr<Standard, Derived> standardTrainer(Tag, Args&&... args);

  // Overriden methods
  EvaluatorPtr<Standard>
  standardEvaluator(const Standard<Sequence>& sequence,
                    bool cached = false) override;

  GeneratorPtr<Standard>
  standardGenerator(RandomNumberGeneratorPtr rng
                      = RNGAdapter<std::mt19937>::make()) override;

  SerializerPtr serializer(TranslatorPtr translator) override;

  // Purely virtual methods
  virtual LogProbability evaluateSymbol(SEPtr<Standard> evaluator,
                                        unsigned int pos,
                                        unsigned int phase) const = 0;

  virtual Standard<Symbol> drawSymbol(SGPtr<Standard> generator,
                                      unsigned int pos,
                                      unsigned int phase,
                                      const Sequence& context) const = 0;

  // Virtual methods
  virtual void initializeCache(CEPtr<Standard> evaluator,
                               unsigned int phase);

  virtual LogProbability evaluateSequence(SEPtr<Standard> evaluator,
                                          unsigned int begin,
                                          unsigned int end,
                                          unsigned int phase) const;

  virtual LogProbability evaluateSymbol(CEPtr<Standard> evaluator,
                                        unsigned int pos,
                                        unsigned int phase) const;
  virtual LogProbability evaluateSequence(CEPtr<Standard> evaluator,
                                          unsigned int begin,
                                          unsigned int end,
                                          unsigned int phase) const;

  virtual Standard<Sequence> drawSequence(SGPtr<Standard> generator,
                                          unsigned int size,
                                          unsigned int phase) const;

  virtual void serialize(SSPtr serializer);

 private:
  // Concrete methods
  DerivedPtr make_shared();
};

}  // namespace model
}  // namespace tops

// Implementation header
#include "model/ProbabilisticModelCrtp.ipp"

#endif  // TOPS_MODEL_PROBABILISTIC_MODEL_
