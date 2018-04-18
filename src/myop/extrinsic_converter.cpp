#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>

#include "nlohmann/json.hpp"

#include "model/Probability.hpp"
#include "model/Segment.hpp"
#include "model/Matrix.hpp"

using namespace std;
using json = nlohmann::json;

// The HintLine has a line of gff hint file
class GtfLine{
 public:
   string _sequence_name;
   string _source;
   string _feature;
   string _start;
   string _end;
   string _score;
   string _strand;
   string _frame;
   string _atribute;

   GtfLine() {
     _sequence_name = "";
     _source = "";
     _feature = "";
     _start = "";
     _end = "";
     _score = "";
     _strand = "";
     _frame = "";
     _atribute = "";
   }

   GtfLine(string sequence_name, string source, string feature, string start, string end, string score,
    string strand, string frame, string atribute) {
     _sequence_name = sequence_name;
     _source = source;
     _feature = feature;
     _start = start;
     _end = end;
     _score = score;
     _strand =strand;
     _frame = frame;
     _atribute = atribute;
   }
};


class ExtrinsicConverter{
 private:
  json _extrinsic_probabilities_contribuition;
  tops::model::Matrix _extrinsic_probabilities;
  
  std::unordered_map<std::string, tops::model::Symbol> _state_indices;
  std::vector<std::string> _state_names;

 public:
  vector<GtfLine> convertGtfFileToGtfLine(const string gtfFile) {
    ifstream gtf_file(gtfFile);
    vector<GtfLine> gtf_line;
    if (!gtf_file.good()) {
      std::cerr << "Error opening " << gtfFile
      << ". Bailing out." << std::endl;
      return gtf_line;
    }

    string line;
    vector<string> tokens;

    while (getline(gtf_file, line)) {
      istringstream iss(line);
      string token;
      while (getline(iss, token, '\t')) {
        tokens.push_back(token);
      }
    }

    // +9 to jump to next line of gff file
    for (size_t i = 0; i < tokens.size(); i+=9) {
      GtfLine n = GtfLine(tokens[i], tokens[i+1], tokens[i+2], tokens[i+3],
           tokens[i+4], tokens[i+5], tokens[i+6], tokens[i+7], tokens[i+8]);
      gtf_line.push_back(n);
    }
    return gtf_line;
  }
  void addContributionEp(const size_t position,
   const string bonus_type, const string type){
    std::vector<std::string> exons = std::vector<std::string> {
      "EI1", "EI2", "E00", "E01", "E02", "E10", "E11", "E12", "E20", 
      "E21", "E22", "ET0", "ET1", "ET2", "rEI0", "rEI1", "rEI2", "rE00",
      "rE01", "rE02", "rE10", "rE11", "rE12", "rE20", "rE21", "rE22",
      "rET0", "rET1", "rET2",
    };
    
    for (const auto& exon : exons) {
      auto d = _extrinsic_probabilities[_state_indices[exon]][position];
      tops::model::Probability ec = double(_extrinsic_probabilities_contribuition[bonus_type][type]);
      _extrinsic_probabilities[_state_indices[exon]][position] *= d * ec;     
    }
  }

  tops::model::Matrix convertGtfLineToProbabilities(
    tops::model::Matrix &probabilities,
    const vector<GtfLine> &gtf_line,
    const string extrinsic_config) {
    
    _extrinsic_probabilities = probabilities;
    initializeMatrixNames();

    std::ifstream ifs(extrinsic_config);
    _extrinsic_probabilities_contribuition = json::parse(ifs);


    for (size_t i = 0; i < gtf_line.size(); i++) {
      GtfLine hl =  gtf_line.at(i);
      string type = hl._feature;
      int start = stoi(hl._start);
      int end = stoi(hl._end);
      if (type.compare("ep") == 0) {
        for (int j = start - 1; j < end; j++) {
          addContributionEp(j, "bonus", "ep");
        }

      } else if (type.compare("exon") == 0) {
        for (int j = start - 1; j < end; j++) {
    //      addContribution(j, "bonus", "exon");
        }
      } else if (type.compare("start") == 0) {
        for (int j = start - 1; j < end; j++) {
      //    addContribution(j, "bonus", "start");
        }
      } else if (type.compare("stop") == 0) {
        for (int j = start - 1; j < end; j++) {
        //  addContribution(j, "bonus", "stop");
        }
      } else if (type.compare("ass") == 0) {
        for (int j = start - 1; j < end; j++) {
          //addContribution(j, "bonus", "ass");
        }
      } else if (type.compare("dss") == 0) {
        for (int j = start - 1; j < end; j++) {
          //addContribution(j, "bonus", "dss");
        }
      } else if (type.compare("intron") == 0) {
        for (int j = start - 1; j < end; j++) {
          //addContribution(j, "bonus", "intron");
        }
      }
    }
    return _extrinsic_probabilities;
  }

  void initializeMatrixNames(){
   
    _state_names = std::vector<std::string>{
      "B",

      "start",
      "ES",
      "EI0", "EI1", "EI2",
      "E00", "E01", "E02",
      "E10", "E11", "E12",
      "E20", "E21", "E22",
      "ET0", "ET1", "ET2",
      "don0", "don1", "don2",
      "I0", "I1", "I2",
      "Is0", "If0", "Is1",
      "If1",  "Is2", "If2",
      "acc0", "acc1", "acc2",
      "stop",

      "N",

      "rstop",
      "rES",
      "rEI0", "rEI1", "rEI2",
      "rE00", "rE01", "rE02",
      "rE10", "rE11", "rE12",
      "rE20", "rE21", "rE22",
      "rET0", "rET1", "rET2",
      "racc0", "racc1", "racc2",
      "rI0", "rI1", "rI2",
      "rIs0", "rIf0", "rIs1",
      "rIf1", "rIs2", "rIf2",
      "rdon0", "rdon1", "rdon2",
      "rstart",

      "E"
    };

    for (std::size_t index = 0; index < _state_names.size(); index++)
      _state_indices.emplace(_state_names[index], index);
  }
};


/*
int main(int argc, char *argv[]) {
  argc = 1;
  argv[0] = NULL;
 
  ExtrinsicConverter *hc = new ExtrinsicConverter();
  vector<GtfLine> gtf_line = hc->convertGtfFileToGtfLine(
      "test.gff");
  tops::model::Probabilities p = hc->convertGtfLineToProbabilities(gtf_line,
      "./src/myop/augustus_config.json", 20);

  for (size_t i = 0; i < p.size(); i++) {
    cout << "p[" << i << "]" << p[i] << endl;
  }
  return 0;
}
*/
