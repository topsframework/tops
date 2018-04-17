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

  // Destructor
  virtual ~ExtrinsicTechnique() = default;

  // Virtual methods
  virtual tops::model::Matrix makeContribuition() = 0;

 protected:
  tops::model::Matrix _probabilities;
};

class NoHints : public ExtrinsicTechnique {
 public:
  explicit NoHints(size_t num_states, size_t sequence_size)
      : ExtrinsicTechnique(tops::model::Matrix(num_states,
          tops::model::Probabilities(sequence_size, 1))) {
  }

  tops::model::Matrix makeContribuition() override {
    return _probabilities;
  }
};

class Augustus : public ExtrinsicTechnique {
 public:
  explicit Augustus(size_t num_states, size_t sequence_size)
      : ExtrinsicTechnique(tops::model::Matrix(num_states,
          tops::model::Probabilities(sequence_size, 1))),
        _sequence_size(sequence_size) {
  }

  tops::model::Matrix makeContribuition() override {
  //  ExtrinsicConverter *hc = new ExtrinsicConverter();
   // vector<GtfLine> gtf_line = hc->convertGtfFileToGtfLine("./src/myop/test.gff");
  //  tops::model::Matrix p = hc->convertGtfLineToProbabilities(gtf_line,
    // "./src/myop/augustus_config.json", _sequence_size);
    tops::model::Matrix p;
    (void) _sequence_size;
    return p;
  }

 private:
  size_t _sequence_size;
};

class Twiscan : public ExtrinsicTechnique {
  
  tops::model::Matrix makeContribuition() override {
    tops::model::Matrix a;
    return a;
  }
  
};

/*
 int main() {

   tops::model::Sequence sequence {
    3, 1, 3, 0, 1, 2, 3, 0, 0, 3, 2};
   ExtrinsicTechniquePtr extrinsic_technique;
   extrinsic_technique
      = std::make_shared<NoHints>(69, sequence.size());
   tops::model::Matrix m = extrinsic_technique->makeContribuition();

  for (size_t i = 0; i < 69; i++) {
    for (size_t j = 0; j < sequence.size(); j++){ 
      cout << m[i][j] << " ";
    }
    cout << endl;
  }
  return 0;
 }
 */
