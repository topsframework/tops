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
#include <cmath>
#include <vector>

// ToPS headers
#include "model/VariableLengthMarkovChain.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                               CONSTRUCTORS                                 */
/*----------------------------------------------------------------------------*/

VariableLengthMarkovChain::VariableLengthMarkovChain(
    ContextTreePtr context_tree) : _context_tree(context_tree) {
}

/*----------------------------------------------------------------------------*/
/*                              STATIC METHODS                                */
/*----------------------------------------------------------------------------*/

VariableLengthMarkovChainPtr VariableLengthMarkovChain::make(
    ContextTreePtr context_tree) {
  return VariableLengthMarkovChainPtr(
    new VariableLengthMarkovChain(context_tree));
}

/*----------------------------------------------------------------------------*/

VariableLengthMarkovChainPtr VariableLengthMarkovChain::trainContextAlgorithm(
    std::vector<Sequence> training_set,
    unsigned int alphabet_size,
    double delta) {

  ContextTreePtr tree = ContextTree::make(alphabet_size);
  tree->initializeContextTreeRissanen(training_set);
  tree->pruneTree(delta);
  tree->removeContextNotUsed();
  tree->normalize();
  return VariableLengthMarkovChain::make(tree);
}

/*----------------------------------------------------------------------------*/

VariableLengthMarkovChainPtr
  VariableLengthMarkovChain::trainFixedLengthMarkovChain(
    std::vector<Sequence> training_set,
    unsigned int order,
    unsigned int alphabet_size,
    double pseudo_counts,
    std::vector<double> weights,
    ProbabilisticModelPtr apriori) {


  ContextTreePtr tree = ContextTree::make(alphabet_size);

  if (apriori) {
    tree->initializeCounter(training_set, order, 0, weights);
    tree->normalize(apriori, pseudo_counts);
  } else {
    tree->initializeCounter(training_set, order, pseudo_counts, weights);
    tree->normalize();
  }

  return VariableLengthMarkovChain::make(tree);
}

/*----------------------------------------------------------------------------*/

VariableLengthMarkovChainPtr
VariableLengthMarkovChain::trainInterpolatedMarkovChain(
    std::vector<Sequence> training_set,
    std::vector<double> weights,
    unsigned int alphabet_size,
    unsigned int order,
    double pseudo_counts,
    ProbabilisticModelPtr apriori) {
  auto tree = ContextTree::make(alphabet_size);

  if (apriori != NULL) {
    tree->initializeCounter(training_set, order, 0, weights);
    tree->pruneTreeSmallSampleSize(400);
    tree->normalize(apriori, pseudo_counts);
  } else {
    tree->initializeCounter(training_set, order, pseudo_counts, weights);
    tree->pruneTreeSmallSampleSize(400);
    tree->normalize();
  }

  return VariableLengthMarkovChain::make(tree);
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

Probability
VariableLengthMarkovChain::evaluateSymbol(SEPtr<Standard> evaluator,
                                          unsigned int pos,
                                          unsigned int /* phase */) const {
  ContextTreeNodePtr c = _context_tree->getContext(evaluator->sequence(), pos);
  if (c == NULL)
    return -std::numeric_limits<double>::infinity();
  else
    return c->getDistribution()->standardEvaluator(
             evaluator->sequence())->evaluateSymbol(pos);
}

/*----------------------------------------------------------------------------*/

Standard<Symbol>
VariableLengthMarkovChain::drawSymbol(SGPtr<Standard> generator,
                                      unsigned int pos,
                                      unsigned int phase,
                                      const Sequence &context) const {
  auto c = _context_tree->getContext(context, pos);

  // TODO(igorbonadio): ERROR!
  if (c == nullptr) return Standard<Symbol>(INVALID_SYMBOL);

  return c->getDistribution()
          ->standardGenerator(generator->randomNumberGenerator())
          ->drawSymbol(pos, phase, context);
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
