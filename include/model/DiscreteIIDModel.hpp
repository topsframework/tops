#ifndef DISCRETE_IID_MODEL_HPP_
#define DISCRETE_IID_MODEL_HPP_

#include <memory>

#include "FactorableModel.hpp"

namespace tops {
  namespace model {
    class DiscreteIIDModel : public FactorableModel {
    public:
      DiscreteIIDModel(const std::vector<double> &log_probabilities);

      virtual int alphabetSize();

      virtual double choose() const;
      virtual Symbol choosePosition(const Sequence &sequence, unsigned int index) const;

      virtual double logProbabilityOf(Symbol symbol);
      virtual double evaluate(const Sequence &sequence, unsigned int begin, unsigned int end);
      virtual double evaluatePosition(const Sequence &sequence, unsigned int index) const;
    };

    typedef std::shared_ptr<DiscreteIIDModel> DiscreteIIDModelPtr;
  }
}

#endif
