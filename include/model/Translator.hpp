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

#ifndef TOPS_MODEL_TRANSLATOR_
#define TOPS_MODEL_TRANSLATOR_

// Standard headers
#include <memory>

// Internal headers
#include "model/MemberDelegator.hpp"
#include "model/HiddenMarkovModelState.hpp"
#include "model/PairHiddenMarkovModelState.hpp"
#include "model/GeneralizedHiddenMarkovModelState.hpp"

namespace tops {
namespace model {

// Forward declarations
class DiscreteIIDModel;
class FixedSequenceAtPosition;
class GeneralizedHiddenMarkovModel;
class HiddenMarkovModel;
class InhomogeneousMarkovChain;
class MaximalDependenceDecomposition;
class MultipleSequentialModel;
class PeriodicInhomogeneousMarkovChain;
class PhasedRunLengthDistribution;
class SimilarityBasedSequenceWeighting;
class TargetModel;
class VariableLengthMarkovChain;

class SignalDuration;
class ExplicitDuration;
class GeometricDuration;

// Forward declaration
class Translator;

/**
 * @typedef TranslatorPtr
 * @brief Alias of pointer to Translator.
 */
// Alias
using TranslatorPtr = std::shared_ptr<Translator>;

/**
 * @class Translator
 * @brief TODO
 */
class Translator : public std::enable_shared_from_this<Translator> {
 public:
  // Alias
  template<typename T>
  using Ptr = std::shared_ptr<T>;

  using HMM = HiddenMarkovModel;
  using PHMM = PairHiddenMarkovModel;
  using GHMM = GeneralizedHiddenMarkovModel;

  // Purely virtual functions
  virtual void translate(Ptr<DiscreteIIDModel> model) = 0;
  virtual void translate(Ptr<FixedSequenceAtPosition> model) = 0;
  virtual void translate(Ptr<GeneralizedHiddenMarkovModel> model) = 0;
  virtual void translate(Ptr<HiddenMarkovModel> model) = 0;
  virtual void translate(Ptr<InhomogeneousMarkovChain> model) = 0;
  virtual void translate(Ptr<MaximalDependenceDecomposition> model) = 0;
  virtual void translate(Ptr<MultipleSequentialModel> model) = 0;
  virtual void translate(Ptr<PeriodicInhomogeneousMarkovChain> model) = 0;
  virtual void translate(Ptr<PhasedRunLengthDistribution> model) = 0;
  virtual void translate(Ptr<SimilarityBasedSequenceWeighting> model) = 0;
  virtual void translate(Ptr<TargetModel> model) = 0;
  virtual void translate(Ptr<VariableLengthMarkovChain> model) = 0;

  virtual void translate(Ptr<typename StateTraits<HMM>::State> state) = 0;
  virtual void translate(Ptr<typename StateTraits<GHMM>::State> state) = 0;
  virtual void translate(Ptr<typename StateTraits<PHMM>::State> state) = 0;

  virtual void translate(Ptr<SignalDuration> duration) = 0;
  virtual void translate(Ptr<ExplicitDuration> duration) = 0;
  virtual void translate(Ptr<GeometricDuration> duration) = 0;

  // Destructor
  virtual ~Translator() = default;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_SYMBOL_
