#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>

#include "nlohmann/json.hpp"

#include "model/Probability.hpp"
#include "model/Segment.hpp"
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
  tops::model::Probabilities _extrinsic_probabilities;
 public:
  vector<GtfLine> convertGtfFileToGtfLine(string gtfFile) {
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
  void addContribution(size_t position, string bonus_type, string type){
    double d = _extrinsic_probabilities[position];
    double ec = _extrinsic_probabilities_contribuition[bonus_type][type];
    d *= ec;
    _extrinsic_probabilities[position] *= d;
  }

  tops::model::Probabilities convertGtfLineToProbabilities(
      const vector<GtfLine> &gtf_line, const string extrinsic_config,
      const size_t sequence_size){
    for (size_t i = 0; i < sequence_size; i++) {
      _extrinsic_probabilities.push_back(1);
    }
    std::ifstream ifs(extrinsic_config);
    _extrinsic_probabilities_contribuition = json::parse(ifs);
    for (size_t i = 0; i < gtf_line.size(); i++) {
      GtfLine hl =  gtf_line.at(i);
      string type = hl._feature;
      int start = stoi(hl._start);
      int end = stoi(hl._end);
      if (type.compare("ep") == 0) {
        for (int j = start - 1; j < end; j++) {
          addContribution(j, "bonus", "ep");
        }

      } else if (type.compare("exon") == 0) {
        for (int j = start - 1; j < end; j++) {
          addContribution(j, "bonus", "exon");
        }
      } else if (type.compare("start") == 0) {
        for (int j = start - 1; j < end; j++) {
          addContribution(j, "bonus", "start");
        }
      } else if (type.compare("stop") == 0) {
        for (int j = start - 1; j < end; j++) {
          addContribution(j, "bonus", "stop");
        }
      } else if (type.compare("ass") == 0) {
        for (int j = start - 1; j < end; j++) {
          addContribution(j, "bonus", "ass");
        }
      } else if (type.compare("dss") == 0) {
        for (int j = start - 1; j < end; j++) {
          addContribution(j, "bonus", "dss");
        }
      } else if (type.compare("intron") == 0) {
        for (int j = start - 1; j < end; j++) {
          addContribution(j, "bonus", "intron");
        }
      }
    }
    return _extrinsic_probabilities;
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
