#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <iostream>

#include "model/Probability.hpp"

class ExtrinsicTechnique;

using ExtrinsicTechniquePtr = std::shared_ptr<ExtrinsicTechnique>;

//using ProbabilitiesPtr = std::shared_ptr<std::vector<tops::model::Probability>>;

class ExtrinsicTechnique {
 public:
  // Constructors
  ExtrinsicTechnique() = default;

  explicit ExtrinsicTechnique(tops::model::Probabilities probabilities)
      : _probabilities(std::move(probabilities)) {
  }

  // Destructor
  virtual ~ExtrinsicTechnique() = default;

  // Virtual methods
  virtual std::vector<tops::model::Probability> makeContribuition() = 0;

 protected:
  std::vector<tops::model::Probability> _probabilities;
};

class NoHints : public ExtrinsicTechnique {
 public:
  explicit NoHints(size_t sequence_size)
      : ExtrinsicTechnique(tops::model::Probabilities(sequence_size, 1)) {
  }

  std::vector<tops::model::Probability> makeContribuition() override {
    return _probabilities;
  }
};

class Augustus : public ExtrinsicTechnique {
  std::vector<tops::model::Probability> makeContribuition() override {
    std::vector<tops::model::Probability> a;
    return a;
  }
};

class Twiscan : public ExtrinsicTechnique {
  std::vector<tops::model::Probability> makeContribuition() override {
    std::vector<tops::model::Probability> a;
    return a;
  }
};

class ExtrinsicInformation {
 public:
  void extrinsicTechnique(ExtrinsicTechniquePtr ec) {
    _extrinsic_technique = std::move(ec);
  }

  ExtrinsicTechniquePtr extrinsicTechnique() {
    return _extrinsic_technique;
  }

 private:
  ExtrinsicTechniquePtr _extrinsic_technique;
};

// int main() {
//   return 0;
// }
