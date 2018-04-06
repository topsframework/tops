#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <iostream>

#include "model/Probability.hpp"
#include "extrinsic_converter.cpp"

class ExtrinsicTechnique;

using ExtrinsicTechniquePtr = std::shared_ptr<ExtrinsicTechnique>;

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
 public:
  explicit Augustus(size_t sequence_size)
     : ExtrinsicTechnique(tops::model::Probabilities(sequence_size, 1)) {
  }

  std::vector<tops::model::Probability> makeContribuition() override {
    ExtrinsicConverter *hc = new ExtrinsicConverter();
    vector<GtfLine> gtf_line = hc->convertGtfFileToGtfLine("test.gff");
    tops::model::Probabilities p = hc->convertGtfLineToProbabilities(gtf_line,
     "./src/myop/augustus_config.json", 20);

    return p;
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
