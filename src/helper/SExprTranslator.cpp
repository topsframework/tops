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

// ToPS headers
#include "model/DiscreteIIDModel.hpp"
#include "model/FixedSequenceAtPosition.hpp"
#include "model/GeneralizedHiddenMarkovModel.hpp"
#include "model/HiddenMarkovModel.hpp"
#include "model/InhomogeneousMarkovChain.hpp"
#include "model/MaximalDependenceDecomposition.hpp"
#include "model/MultipleSequentialModel.hpp"
#include "model/PhasedInhomogeneousMarkovChain.hpp"
#include "model/PhasedRunLengthDistribution.hpp"
#include "model/SimilarityBasedSequenceWeighting.hpp"
#include "model/TargetModel.hpp"
#include "helper/SExprTranslator.hpp"

namespace tops {
namespace helper {

void SExprTranslator::translate(Ptr<model::DiscreteIIDModel> model) {
  _sexpr = "(DiscreteIIDModel:";
  for (auto p : model->probabilities()) {
    _sexpr += " " + std::to_string(p);
  }
  _sexpr += ")";
}

void SExprTranslator::translate(Ptr<model::FixedSequenceAtPosition> /* model */) {

}

void SExprTranslator::translate(Ptr<model::GeneralizedHiddenMarkovModel> /* model */) {

}

void SExprTranslator::translate(Ptr<model::HiddenMarkovModel> /* model */) {

}

void SExprTranslator::translate(Ptr<model::InhomogeneousMarkovChain> /* model */) {

}

void SExprTranslator::translate(Ptr<model::MaximalDependenceDecomposition> /* model */) {

}

void SExprTranslator::translate(Ptr<model::MultipleSequentialModel> /* model */) {

}

void SExprTranslator::translate(Ptr<model::PhasedInhomogeneousMarkovChain> /* model */) {

}

void SExprTranslator::translate(Ptr<model::PhasedRunLengthDistribution> /* model */) {

}

void SExprTranslator::translate(Ptr<model::SimilarityBasedSequenceWeighting> /* model */) {

}

void SExprTranslator::translate(Ptr<model::TargetModel> /* model */) {

}

void SExprTranslator::translate(Ptr<model::VariableLengthMarkovChain> /* model */) {

}


}  // namespace helper
}  // namespace tops