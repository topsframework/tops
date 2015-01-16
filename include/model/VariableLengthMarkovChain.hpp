#ifndef VARIABLE_LENGTH_MARKOV_CHAIN_HPP_
#define VARIABLE_LENGTH_MARKOV_CHAIN_HPP_

#include "FactorableModel.hpp"
#include "ContextTree.hpp"

namespace tops {
  namespace model {
    class VariableLengthMarkovChain : public FactorableModel {
      VariableLengthMarkovChain(ContextTreePtr tree, int alphabetSize);

      virtual int alphabetSize();

      virtual double evaluate(const Sequence &sequence, unsigned int begin, unsigned int end);
      virtual double evaluatePosition(const Sequence &sequence, unsigned int index) const;

      virtual double choose() const;
      virtual Symbol choosePosition(const Sequence &sequence, unsigned int index) const;
    };
  }
}

#endif
