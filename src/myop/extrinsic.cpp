#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <iostream>

#include "model/Matrix.hpp"
#include "extrinsic_converter.cpp"

class ExtrinsicTechnique;

using ExtrinsicTechniquePtr = std::shared_ptr<ExtrinsicTechnique>;

class ExtrinsicTechnique {
 public:
  // Constructors
  ExtrinsicTechnique() = default;

  explicit ExtrinsicTechnique(tops::model::Matrix probabilities)
      : _probabilities(std::move(probabilities)) {
  }

  void printExtrinsicMatrix(){
    for (size_t ii = 0; ii < 69; ii++) {
      for (size_t jj = 0; jj < _probabilities[ii].size(); jj++){ 
        cout << _probabilities[ii][jj] << " ";
      }
      cout << endl;
    }
  }

  // Destructor
  virtual ~ExtrinsicTechnique() = default;

  // Virtual methods
  virtual tops::model::Matrix makeContribuition() = 0;

 protected:
  tops::model::Matrix _probabilities;
};

class NoHints : public ExtrinsicTechnique {
 public:
  explicit NoHints(const size_t num_states, const size_t sequence_size)
      : ExtrinsicTechnique(tops::model::Matrix(num_states,
          tops::model::Probabilities(sequence_size, 1))) {
  }

  tops::model::Matrix makeContribuition() override {
    return _probabilities;
  }
};

class Augustus : public ExtrinsicTechnique {
 public:
  explicit Augustus(const size_t num_states, const size_t sequence_size)
      : ExtrinsicTechnique(tops::model::Matrix(num_states,
          tops::model::Probabilities(sequence_size, 1))) {
  }

  tops::model::Matrix makeContribuition() override {
    std::unique_ptr<ExtrinsicConverter> hc = std::make_unique<ExtrinsicConverter>();
    vector<GtfLine> gtf_line = hc->convertGtfFileToGtfLine("./src/myop/test.gff");
    _probabilities = hc->convertGtfLineToProbabilities(_probabilities, gtf_line, "./src/myop/augustus_config.json");
    return _probabilities;
  }
};

class Twinscan : public ExtrinsicTechnique {
  public:
  explicit Twinscan(const size_t num_states, const size_t sequence_size)
      : ExtrinsicTechnique(tops::model::Matrix(num_states,
          tops::model::Probabilities(sequence_size, 1))) {
  }
  
  tops::model::Matrix makeContribuition() override {
    return _probabilities;
  }
};
/*
int main() {

  tops::model::Sequence sequence {
  3, 1, 3, 0, 1, 2, 3, 0, 0, 3, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,};
  ExtrinsicTechniquePtr extrinsic_technique;
  extrinsic_technique
    = std::make_shared<Augustus>(69, sequence.size());
  tops::model::Matrix probabilities = extrinsic_technique->makeContribuition();

  extrinsic_technique->printExtrinsicMatrix();
  return 0;
}
 */