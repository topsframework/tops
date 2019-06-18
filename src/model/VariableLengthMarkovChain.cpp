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

// Interface header
#include "model/VariableLengthMarkovChain.hpp"

// Standard headers
#include <cmath>
#include <limits>
#include <vector>

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

/*================================  TRAINER  =================================*/

VariableLengthMarkovChainPtr
VariableLengthMarkovChain::train(TrainerPtr<Multiple, Self> trainer,
                                 context_algorithm,
                                 size_t alphabet_size,
                                 double delta) {
  ContextTreePtr tree = ContextTree::make(alphabet_size);
  tree->initializeContextTreeRissanen(trainer->training_set());
  tree->pruneTree(delta);
  tree->removeContextNotUsed();
  tree->normalize();
  return VariableLengthMarkovChain::make(tree);
}

/*----------------------------------------------------------------------------*/

VariableLengthMarkovChainPtr
VariableLengthMarkovChain::train(TrainerPtr<Multiple, Self> trainer,
                                 fixed_length_algorithm,
                                 size_t order,
                                 size_t alphabet_size,
                                 double pseudo_counts,
                                 std::vector<double> weights,
                                 ProbabilisticModelPtr apriori) {
  ContextTreePtr tree = ContextTree::make(alphabet_size);

  if (apriori) {
    tree->initializeCounter(trainer->training_set(),
                            order, 0, weights);
    tree->normalize(apriori, pseudo_counts);
  } else {
    tree->initializeCounter(trainer->training_set(),
                            order, pseudo_counts, weights);
    tree->normalize();
  }

  return VariableLengthMarkovChain::make(tree);
}

/*----------------------------------------------------------------------------*/

VariableLengthMarkovChainPtr
VariableLengthMarkovChain::train(TrainerPtr<Multiple, Self> trainer,
                                 interpolation_algorithm,
                                 std::vector<double> weights,
                                 size_t alphabet_size,
                                 size_t order,
                                 double pseudo_counts,
                                 ProbabilisticModelPtr apriori) {
  auto tree = ContextTree::make(alphabet_size);

  if (apriori != nullptr) {
    tree->initializeCounter(trainer->training_set(),
                            order, 0, weights);
    tree->pruneTreeSmallSampleSize(400);
    tree->normalize(apriori, pseudo_counts);
  } else {
    tree->initializeCounter(trainer->training_set(),
                            order, pseudo_counts, weights);
    tree->pruneTreeSmallSampleSize(400);
    tree->normalize();
  }

  return VariableLengthMarkovChain::make(tree);
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  EVALUATOR  ================================*/

Probability
VariableLengthMarkovChain::evaluateSymbol(SEPtr<Multiple> evaluator,
                                          size_t pos,
                                          size_t /* phase */) const {
  auto c = _context_tree->getContext(evaluator->sequence()[0], pos);

  if (c == nullptr) return 0;
  return c->getDistribution()
          ->standardEvaluator(evaluator->sequence())
          ->evaluateSymbol(pos);
}

/*----------------------------------------------------------------------------*/

Probability VariableLengthMarkovChain::evaluateSequence(
    SEPtr<Multiple> evaluator,
    size_t begin,
    size_t end,
    size_t phase) const {
  return Base::evaluateSequence(evaluator, begin, end, phase);
}

/*----------------------------------------------------------------------------*/

void VariableLengthMarkovChain::initializeCache(CEPtr<Multiple> evaluator,
                                                size_t phase) {
  Base::initializeCache(evaluator, phase);
}

/*----------------------------------------------------------------------------*/

Probability
VariableLengthMarkovChain::evaluateSymbol(CEPtr<Multiple> evaluator,
                                          size_t pos,
                                          size_t phase) const {
  return Base::evaluateSymbol(evaluator, pos, phase);
}

/*----------------------------------------------------------------------------*/

Probability VariableLengthMarkovChain::evaluateSequence(
    CEPtr<Multiple> evaluator,
    size_t begin,
    size_t end,
    size_t phase) const {
  return Base::evaluateSequence(evaluator, begin, end, phase);
}

/*===============================  GENERATOR  ================================*/

Multiple<Symbol>
VariableLengthMarkovChain::drawSymbol(SGPtr<Multiple> generator,
                                      size_t pos,
                                      size_t phase,
                                      const Multiple<Sequence>& context) const {
  auto c = _context_tree->getContext(context[0], pos);

  // TODO(igorbonadio): ERROR!
  if (c == nullptr) return Multiple<Symbol>(INVALID_SYMBOL);

  return c->getDistribution()
          ->standardGenerator(generator->randomNumberGenerator())
          ->drawSymbol(pos, phase, context);
}

/*----------------------------------------------------------------------------*/

Multiple<Sequence> VariableLengthMarkovChain::drawSequence(
    SGPtr<Multiple> generator,
    size_t size,
    size_t phase) const {
  return Base::drawSequence(generator, size, phase);
}

/*===============================  SERIALIZER  ===============================*/

void VariableLengthMarkovChain::serialize(SSPtr serializer) {
  Base::serialize(serializer);
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
