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

  tops::model::Probabilities convertGtfLineToProbabilities(
      const vector<GtfLine> &gtf_line, const string extrinsic_config,
      const size_t sequence_size){
      
    tops::model::Probabilities extrinsic_probabilities;
    
    for (size_t i = 0; i < sequence_size; i++) {
      extrinsic_probabilities.push_back(1);
    }
    std::ifstream ifs(extrinsic_config);
    json extrinsic_probabilities_contribuition = json::parse(ifs);

    for (size_t i = 0; i < gtf_line.size(); i++) {
      GtfLine hl =  gtf_line.at(i);
      string type = hl._feature;
      int start = stoi(hl._start);
      int end = stoi(hl._end);
      
      if (type.compare("ep") == 0) {
        for (int j = start - 1; j < end; j++) {
          double d = extrinsic_probabilities[j];
          double ec = extrinsic_probabilities_contribuition["bonus"]["ep"];
          d *= ec;
          extrinsic_probabilities[j] *= d;
        }

      }else if (type.compare("exon") == 0) {
        for (int j = start - 1; j < end; j++) {
          double d = extrinsic_probabilities[j];
          double ec = extrinsic_probabilities_contribuition["bonus"]["exon"];
          d *= ec;
          extrinsic_probabilities[j] *= d;
        }
      }else if (type.compare("start") == 0) {
        for (int j = start - 1; j < end; j++) {
          double d = extrinsic_probabilities[j];
          double ec = extrinsic_probabilities_contribuition["bonus"]["start"];
          d *= ec;
          extrinsic_probabilities[j] *= d;
        }
      }else if (type.compare("stop") == 0) {
        for (int j = start - 1; j < end; j++) {
          double d = extrinsic_probabilities[j];
          double ec = extrinsic_probabilities_contribuition["bonus"]["stop"];
          d *= ec;
          extrinsic_probabilities[j] *= d;
        }
      }else if (type.compare("ass") == 0) {
        for (int j = start - 1; j < end; j++) {
          double d = extrinsic_probabilities[j];
          double ec = extrinsic_probabilities_contribuition["bonus"]["ass"];
          d *= ec;
          extrinsic_probabilities[j] *= d;
        }
      }else if (type.compare("dss") == 0) {
        for (int j = start - 1; j < end; j++) {
          double d = extrinsic_probabilities[j];
          double ec = extrinsic_probabilities_contribuition["bonus"]["dss"];
          d *= ec;
          extrinsic_probabilities[j] *= d;
        }
      }else if (type.compare("intron") == 0) {
        for (int j = start - 1; j < end; j++) {
          double d = extrinsic_probabilities[j];
          double ec = extrinsic_probabilities_contribuition["bonus"]["intron"];
          d *= ec;
          extrinsic_probabilities[j] *= d;
        }
      }
    
    }
    return extrinsic_probabilities;
  }
};
