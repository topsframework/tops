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

// ToPS headers
#include "VariableLengthMarkovChain.hpp"

namespace tops {
namespace model {

VariableLengthMarkovChainPtr VariableLengthMarkovChain::make(
    ContextTreePtr context_tree) {
  return VariableLengthMarkovChainPtr(
    new VariableLengthMarkovChain(context_tree));
}

VariableLengthMarkovChain::VariableLengthMarkovChain(
    ContextTreePtr context_tree) : _context_tree(context_tree) {
}

VariableLengthMarkovChainPtr VariableLengthMarkovChain::trainContextAlgorithm(
      std::vector<Sequence> training_set,
      unsigned int alphabet_size,
      double delta) {

  ContextTreePtr tree = ContextTree::make(alphabet_size);
  tree->initializeContextTreeRissanen(training_set);
  tree->pruneTree(delta);
  tree->removeContextNotUsed();
  tree->normalize();
  VariableLengthMarkovChainPtr m = VariableLengthMarkovChain::make(tree);
  double loglikelihood = 0.0;
  unsigned int sample_size = 0;
  for (int i = 0; i < (int) training_set.size(); i++) {
          loglikelihood
            += m->evaluateSequence(training_set[i], 0, training_set[i].size() - 1);
          sample_size += training_set[i].size();
  }
  return m;
}

VariableLengthMarkovChainPtr VariableLengthMarkovChain::trainFixedLengthMarkovChain(
      std::vector<Sequence> training_set,
      unsigned int order,
      unsigned int alphabet_size,
      double pseudo_counts,
      std::vector<double> weights,
      ProbabilisticModelPtr apriori) {


  ContextTreePtr tree = ContextTree::make(alphabet_size);

  if(apriori){
    tree->initializeCounter(training_set, order, 0, weights);
    tree->normalize(apriori, pseudo_counts);
  } else {
    tree->initializeCounter(training_set, order, pseudo_counts, weights);
    tree->normalize();
  }

  VariableLengthMarkovChainPtr m = VariableLengthMarkovChain::make(tree);
  double loglikelihood = 0.0;
  unsigned int sample_size = 0;
  for (int i = 0; i < (int) training_set.size(); i++) {
    loglikelihood
      += m->evaluateSequence(training_set[i], 0, training_set[i].size() - 1);
    sample_size += training_set[i].size();
  }

  return m;

}

int VariableLengthMarkovChain::alphabetSize() const {
  return _context_tree->alphabetSize();
}

double VariableLengthMarkovChain::evaluatePosition(const Sequence &s,
                                                   unsigned int i) const {
  ContextTreeNodePtr c = _context_tree->getContext(s, i);
  if (c == NULL)
    return -HUGE;
  else
    return c->getDistribution()->probabilityOf(s[i]);
}

Symbol VariableLengthMarkovChain::choosePosition(const Sequence &s,
                                              unsigned int i) const {
  ContextTreeNodePtr c = _context_tree->getContext(s, i);
  if (c == NULL)
    // TODO(igorbonadio): ERROR!
    return 0;
  else
    return c->getDistribution()->evaluatePosition(s, i);
}


}  // namespace model
}  // namespace tops
