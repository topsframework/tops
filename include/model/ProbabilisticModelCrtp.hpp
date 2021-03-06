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
using ProbabilisticModelCrtpPtr
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
    std::vector<Probability> prefix_sum_array;
  };

  // Aliases
  using Base = void;
  using Self = ProbabilisticModelCrtp<Derived>;
  using SelfPtr = ProbabilisticModelCrtpPtr<Derived>;
  using DerivedPtr = std::shared_ptr<Derived>;

  template<template<typename Target> class Decorator>
  using SEPtr = SimpleEvaluatorPtr<Decorator, Derived>;
  template<template<typename Target> class Decorator>
  using CEPtr = CachedEvaluatorPtr<Decorator, Derived>;

  template<template<typename Target> class Decorator>
  using SGPtr = SimpleGeneratorPtr<Decorator, Derived>;

  using SSPtr = SimpleSerializerPtr<Derived>;

  /*============================[ STATIC METHODS ]============================*/

  // Factories

  /**
   * Make a std::shared_ptr<Derived>.
   * @param args Any arguments, forwared to Derived's constructor
   * @return New instance of new std::shared_ptr<Derived>
   */
  template<typename... Args>
  static DerivedPtr make(Args&&... args);

  // Trainer

  /**
   * Factory of Simple Trainers for non-supervised learning
   * of Derived's parameters.
   * @return New instance of TrainerPtr<Standard, Derived>
   */
  static TrainerPtr<Standard, Derived> standardTrainer();

  /**
   * Factory of Fixed Trainers for non-supervised learning
   * of Derived's parameters.
   * @param model Trained model with predefined parameters
   * @return New instance of TrainerPtr<Standard, Derived>
   */
  static TrainerPtr<Standard, Derived> standardTrainer(DerivedPtr model);

  /**
   * Factory of Cached Trainers for non-supervised learning
   * of Derived's parameters.
   * @param tag Tag representing the training algorithm
   * @param params Parameters for the training algorithn chosen
   * @return New instance of TrainerPtr<Standard, Derived>
   */
  template<typename Tag, typename... Args>
  static TrainerPtr<Standard, Derived> standardTrainer(Tag, Args&&... args);

  /*==========================[ OVERRIDEN METHODS ]===========================*/
  /*-------------------------( Probabilistic Model )--------------------------*/

  EvaluatorPtr<Standard>
  standardEvaluator(const Standard<Sequence>& sequence,
                    bool cached = false) override;

  GeneratorPtr<Standard>
  standardGenerator(RandomNumberGeneratorPtr rng
                      = RNGAdapter<std::mt19937>::make()) override;

  SerializerPtr serializer(TranslatorPtr translator) override;

  /*========================[ PURELY VIRTUAL METHODS ]========================*/

  // SimpleEvaluator

  /**
   * Evaluates (given the trained model, returns the probability of)
   * a symbol of a SimpleEvaluator's standard sequence (**without a cache**).
   * @param evaluator Instance of a SimpleEvaluator
   * @param pos Position within the full sequence
   * @param phase Phase of the full sequence
   * @return \f$Pr(s[pos])\f$
   */
  virtual Probability evaluateSymbol(SEPtr<Standard> evaluator,
                                     unsigned int pos,
                                     unsigned int phase) const = 0;

  /**
   * Evaluates (given the trained model, returns the probability of)
   * a subsequence of a SimpleEvaluator's sequence (**without a cache**).
   * @param evaluator Instance of a SimpleEvaluator
   * @param begin Position of the beginning of the subsequence
   * @param end Position of the end of the subsequence, minus 1
   * @param phase Phase of the full sequence
   * @return \f$Pr(s[begin..end-1])\f$
   */
  virtual Probability evaluateSequence(SEPtr<Standard> evaluator,
                                       unsigned int begin,
                                       unsigned int end,
                                       unsigned int phase) const = 0;

  // CachedEvaluator

  /**
   * Lazily initializes the cache of a CachedEvaluator.
   * @param evaluator Instance of CachedEvaluator
   * @param phase Phase of the full sequence
   */
  virtual void initializeCache(CEPtr<Standard> evaluator,
                               unsigned int phase) = 0;

  /**
   * Evaluates (given the trained model, returns the probability of)
   * a symbol of a CachedEvaluator's sequence (**with a cache**).
   * @param evaluator Instance of a CachedEvaluator
   * @param pos Position within the full sequence
   * @param phase Phase of the full sequence
   * @return \f$Pr(s[pos])\f$
   */
  virtual Probability evaluateSymbol(CEPtr<Standard> evaluator,
                                     unsigned int pos,
                                     unsigned int phase) const = 0;

  /**
   * Evaluates (given the trained model, returns the probability of)
   * a subsequence of a CachedEvaluator's sequence (**with a cache**).
   * @param evaluator Instance of a CachedEvaluator
   * @param begin Position of the beginning of the subsequence
   * @param end Position of the end of the subsequence, minus 1
   * @param phase Phase of the full sequence
   * @return \f$Pr(s[begin..end-1])\f$
   */
  virtual Probability evaluateSequence(CEPtr<Standard> evaluator,
                                       unsigned int begin,
                                       unsigned int end,
                                       unsigned int phase) const = 0;

  // SimpleGenerator

  /**
   * Draws (given the trained model, randomly choose) a symbol
   * with a SimpleGenerator (**with no cache**).
   * @param generator Instance of SimpleGenerator
   * @param pos Position of the symbol to be generated
   * @param phase Phase of the sequence in wich the symbol belongs
   * @param context Context to be considered when generating the symbol
   * @return \f$x,\ x \in X\f$
   */
  virtual Standard<Symbol> drawSymbol(SGPtr<Standard> generator,
                                      unsigned int pos,
                                      unsigned int phase,
                                      const Sequence& context) const = 0;

  /**
   * Draws (given the trained model, randomly choose) a sequence
   * with a SimpleGenerator (**with no cache**).
   * @param generator Instance of SimpleGenerator
   * @param pos Size of the sequence to be generated
   * @param phase Phase of the sequence to be generated
   * @return \f$x,\ x \in X\f$
   */
  virtual Standard<Sequence> drawSequence(SGPtr<Standard> generator,
                                          unsigned int size,
                                          unsigned int phase) const = 0;

  // SimpleSerializer

  /**
   * Serializes (given the trained model, save its parameters
   * in the disk using some file format) with a SimpleSerializer.
   * @param serializer Instance of SimpleSerializer
   */
  virtual void serialize(SSPtr serializer) = 0;

 private:
  // Concrete methods
  DerivedPtr make_shared();
};

}  // namespace model
}  // namespace tops

// Implementation header
#include "model/ProbabilisticModelCrtp.ipp"

#endif  // TOPS_MODEL_PROBABILISTIC_MODEL_
