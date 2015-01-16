#include "DiscreteIIDModel.hpp"

#include <string>

#include "src/DiscreteIIDModel.hpp"

namespace tops {
  namespace model {
    DiscreteIIDModel::DiscreteIIDModel(const std::vector<double> &log_probabilities) {
      auto model = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(log_probabilities));
      auto alphabet = tops::AlphabetPtr(new Alphabet());
      for (auto p : log_probabilities) {
        alphabet->createSymbol(std::to_string(p));
      }
      model->setAlphabet(alphabet);
      setTopsModel(model);
    }

    int DiscreteIIDModel::alphabetSize() {
      return getTopsModel()->alphabet()->size();
    }

    double DiscreteIIDModel::choose() const {
      return getTopsModel()->choose();
    }

    Symbol DiscreteIIDModel::choosePosition(const Sequence &sequence, unsigned int index) const {
      return getTopsModel()->factorable()->choosePosition(sequence, index);
    }

    double DiscreteIIDModel::logProbabilityOf(Symbol symbol) {
      return getTopsModel()->log_probability_of(symbol);
    }

    double DiscreteIIDModel::evaluate(const Sequence &sequence, unsigned int begin, unsigned int end) {
      return getTopsModel()->evaluate(sequence, begin, end);
    }

    double DiscreteIIDModel::evaluatePosition(const Sequence &sequence, unsigned int index) const {
      return getTopsModel()->evaluatePosition(sequence, index);
    }
  }
}
