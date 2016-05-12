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

#ifndef TOPS_HELPER_SEXPR_TRANSLATOR_
#define TOPS_HELPER_SEXPR_TRANSLATOR_

// Standard headers
#include <memory>
#include <string>

// Internal headers
#include "model/DiscreteIIDModel.hpp"
#include "model/FixedSequenceAtPosition.hpp"
#include "model/GeneralizedHiddenMarkovModel.hpp"
#include "model/HiddenMarkovModel.hpp"
#include "model/InhomogeneousMarkovChain.hpp"
#include "model/MaximalDependenceDecomposition.hpp"
#include "model/MultipleSequentialModel.hpp"
#include "model/PeriodicInhomogeneousMarkovChain.hpp"
#include "model/PhasedRunLengthDistribution.hpp"
#include "model/SimilarityBasedSequenceWeighting.hpp"
#include "model/TargetModel.hpp"

#include "model/SignalDuration.hpp"
#include "model/ExplicitDuration.hpp"
#include "model/GeometricDuration.hpp"

// Tested header
#include "model/Translator.hpp"

namespace tops {
namespace helper {

// Forward declaration
class SExprTranslator;

// Alias
using SExprTranslatorPtr
  = std::shared_ptr<SExprTranslator>;

class SExprTranslator : public model::Translator {
 public:
  // Alias
  template<typename T>
  using Ptr = std::shared_ptr<T>;

  using Self = SExprTranslator;
  using SelfPtr = std::shared_ptr<Self>;

  // Static methods
  template<typename... Args>
  static SelfPtr make(Args... args) {
    return std::shared_ptr<Self>(new Self(std::forward<Args>(args)...));
  }

  // Overriden methods
  void translate(Ptr<model::DiscreteIIDModel> model) override;
  void translate(Ptr<model::FixedSequenceAtPosition> model) override;
  void translate(Ptr<model::GeneralizedHiddenMarkovModel> model) override;
  void translate(Ptr<model::HiddenMarkovModel> model) override;
  void translate(Ptr<model::InhomogeneousMarkovChain> model) override;
  void translate(Ptr<model::MaximalDependenceDecomposition> model) override;
  void translate(Ptr<model::MultipleSequentialModel> model) override;
  void translate(Ptr<model::PeriodicInhomogeneousMarkovChain> model) override;
  void translate(Ptr<model::PhasedRunLengthDistribution> model) override;
  void translate(Ptr<model::SimilarityBasedSequenceWeighting> model) override;
  void translate(Ptr<model::TargetModel> model) override;
  void translate(Ptr<model::VariableLengthMarkovChain> model) override;

  void translate(
    Ptr<typename model::HiddenMarkovModel::State> state) override;
  void translate(
    Ptr<typename model::GeneralizedHiddenMarkovModel::State> state) override;

  void translate(Ptr<model::SignalDuration> duration) override;
  void translate(Ptr<model::ExplicitDuration> duration) override;
  void translate(Ptr<model::GeometricDuration> duration) override;

  // Concrete mehtods
  std::string sexpr();
  SelfPtr make_shared();

 private:
  // Instance variables
  std::string _sexpr;
};

}  // namespace helper
}  // namespace tops

#endif  // TOPS_HELPER_SEXPR_TRANSLATOR_
