#include "VariableLengthMarkovChain.hpp"

#include "src/VariableLengthMarkovChain.hpp"

namespace tops {
  namespace model {
    VariableLengthMarkovChain::VariableLengthMarkovChain(ContextTreePtr tree, int alphabetSize) {
      _self = tops::VariableLengthMarkovChainPtr(new tops::VariableLengthMarkovChain(tree->_self));
      auto alphabet = tops::AlphabetPtr(new Alphabet());
      for (int i = 0; i < alphabetSize; i++) {
        alphabet->createSymbol(std::to_string(i));
      }
      _self->setAlphabet(alphabet);
    }

    int VariableLengthMarkovChain::alphabetSize() {
      return _self->alphabet()->size();
    }

    double VariableLengthMarkovChain::evaluate(const Sequence &sequence, unsigned int begin, unsigned int end) {
      return _self->evaluate(sequence, begin, end);
    }

    double VariableLengthMarkovChain::evaluatePosition(const Sequence &sequence, unsigned int index) const {
      return _self->evaluatePosition(sequence, index);
    }

    double VariableLengthMarkovChain::choose() const {
      return _self->choose();
    }

    Symbol VariableLengthMarkovChain::choosePosition(const Sequence &sequence, unsigned int index) const {
      return _self->factorable()->choosePosition(sequence, index);
    }
  }
}
