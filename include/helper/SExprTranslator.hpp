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

// ToPS headers
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
  template<typename... Ts>
  static SelfPtr make(Ts... args) {
    return std::shared_ptr<Self>(new Self(std::forward<Ts>(args)...));
  }

  // Overriden methods
  void translate(Ptr<model::DiscreteIIDModel> model) override;
  void translate(Ptr<model::FixedSequenceAtPosition> model) override;
  void translate(Ptr<model::GeneralizedHiddenMarkovModel> model) override;
  void translate(Ptr<model::HiddenMarkovModel> model) override;
  void translate(Ptr<model::InhomogeneousMarkovChain> model) override;
  void translate(Ptr<model::MaximalDependenceDecomposition> model) override;
  void translate(Ptr<model::MultipleSequentialModel> model) override;
  void translate(Ptr<model::PhasedInhomogeneousMarkovChain> model) override;
  void translate(Ptr<model::PhasedRunLengthDistribution> model) override;
  void translate(Ptr<model::SimilarityBasedSequenceWeighting> model) override;
  void translate(Ptr<model::TargetModel> model) override;
  void translate(Ptr<model::VariableLengthMarkovChain> model) override;

  void translate(Ptr<model::HiddenMarkovModelState> state) override;

  // Concrete mehtods
  std::string sexpr() {
    return _sexpr;
  }

  SelfPtr make_shared();

 private:
  // Instance variables
  std::string _sexpr;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_SYMBOL_
