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
#include "model/Sequence.hpp"

// Tested header VLMC
#include "model/VariableLengthMarkovChain.hpp"
#include "helper/VariableLengthMarkovChain.hpp"
#include "model/Standard.hpp"
#include "model/CachedEvaluator.hpp"


using namespace std;
using json = nlohmann::json;
using VLMCPtr = tops::model::VariableLengthMarkovChainPtr;
using VLMC = tops::model::VariableLengthMarkovChain;

namespace tops{
  namespace myop{
    extern tops::model::Sequence conservation_sequence;
  }
}

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

   GtfLine(string sequence_name, string source,
    string feature, string start, string end, string score,
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

class FastaSequence{
  private:
  string _sequenceName;
  string _sequenceValue;

  public:
  FastaSequence(string sequenceName, string sequenceValue){
    _sequenceName = sequenceName;
    _sequenceValue = sequenceValue;
  }

  FastaSequence(){
  }

  void setSequenceName(string sequenceName){
    _sequenceName = sequenceName;
  }

  void setSequenceValue(string sequenceValue){
    _sequenceValue = sequenceValue;
  }

  string getSequenceName(){
    return _sequenceName;
  }

  string getSequenceValue(){
    return _sequenceValue;
  }
};

//based on Rosetta http://rosettacode.org/wiki/FASTA_format#C%2B%2B
class FastaConverter{
  public:
    vector<FastaSequence> converteFastaFileToFastaSequences(string fastaFile){
      vector<FastaSequence> fastaSequences;

      if(fastaFile.empty()){
        std::cerr << "Usage: '"<< fastaFile <<"' [fasta infile]" << std::endl;
        return fastaSequences;
      }

      std::ifstream input(fastaFile);
      if(!input.good()){
        std::cerr << "Error opening " << fastaFile << ". Bailing out." << std::endl;
        return fastaSequences;
      }

      std::string line, name, content;
      FastaSequence fs;
      while( std::getline( input, line ).good() ){
        if( line.empty() || line[0] == '>' ){ // Identifier marker

          if( !name.empty() ){ // Print out what we read from the last entry
            fs.setSequenceName(name);
            fs.setSequenceValue(content);
            fastaSequences.push_back(fs);
            name.clear();
          }
          if( !line.empty() ){
            name = line.substr(1);
            fs.setSequenceName(name);
          }
          content.clear();
        }else if( !name.empty() ){
          if( line.find(' ') != std::string::npos ){ // Invalid sequence--no spaces allowed
            name.clear();
            content.clear();
          }else{
             content += line;
          }
        }
      }

    if( !name.empty() ){ // Print out what we read from the last entry
      fs.setSequenceName(name);
      fs.setSequenceValue(content);
      fastaSequences.push_back(fs);
    }

    return fastaSequences;
    }

    tops::model::Sequence convertFastaSequenceToToPSSequence(FastaSequence fasta_sequence){
      //ATAATAACTTG A 0, C 1, G 2, T 3
      string s = fasta_sequence.getSequenceValue();
      tops::model::Sequence tops_sequence;
      std::unordered_map<char, tops::model::Symbol> map;
      map.emplace('A', 0);
      map.emplace('C', 1);
      map.emplace('G', 2);
      map.emplace('T', 3);

      for (auto c : s)
        tops_sequence.push_back(map[c]);
      return tops_sequence;

    }

    tops::model::Sequence convertFastaConservationSequenceToToPSSequence(FastaSequence fasta_sequence){
      //ATAATAACTTG A 0, C 1, G 2, T 3
      string s = fasta_sequence.getSequenceValue();
      tops::model::Sequence tops_sequence;

      std::unordered_map<char, tops::model::Symbol> map;
      map.emplace('0', 0);
      map.emplace('1', 1);
      map.emplace('2', 2);

      for (auto c : s)
        tops_sequence.push_back(map[c]);
      return tops_sequence;
    }


};

class ExtrinsicConverter{
 protected:
  tops::model::Matrix _extrinsic_probabilities;
  std::unordered_map<std::string, tops::model::Symbol>
   _states_indices;
  std::vector<std::string> _states_names;

  private:
  void initializeMatrixNames(std::vector<std::string>
                            &states_names,
   std::unordered_map<std::string, tops::model::Symbol>
                            &states_indices ){
   
    states_names = std::vector<std::string>{
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

    for (std::size_t index = 0; index < states_names.size(); index++)
      states_indices.emplace(states_names[index], index);
  }
  public:
    ExtrinsicConverter(){
      initializeMatrixNames(_states_names, _states_indices);
    }
  
    void printAllHints(){
      for (const auto& lines : _extrinsic_probabilities){
        for (const auto& coluns : lines){
          std::cout << coluns << " ";
        }
        std::cout << std::endl;
      }
    }

    //select states that will receive extrinsic contribution
    std::vector<std::string>
    selectStatesToReceiveContribution(const string type){    
        std::vector<std::string> exons;
        if ((type.compare("ep") == 0) || (type.compare("exon") == 0)) {
        exons = std::vector<std::string> {
          "EI0", "EI1", "EI2",
          "E00", "E01", "E02",
          "E10", "E11", "E12",
          "E20", "E21", "E22",
          "ET0", "ET1", "ET2",
          "rEI0", "rEI1", "rEI2",
          "rE00", "rE01", "rE02",
          "rE10", "rE11", "rE12",
          "rE20", "rE21", "rE22",
          "rET0", "rET1", "rET2",
          "ES", "rES",
        };
      } else if (type.compare("start") == 0) {
        exons = std::vector<std::string> {
          "start", "rstart",
        };
      } else if (type.compare("stop") == 0) {
        exons = std::vector<std::string> {
          "stop", "rstop",
        };
      } else if (type.compare("ass") == 0) {
        exons = std::vector<std::string> {
          "acc0", "acc1", "acc2", "racc0", "racc1", "racc2",
        };
      } else if (type.compare("dss") == 0) {
        exons = std::vector<std::string> {
          "don0", "don1", "don2", "rdon0", "rdon1", "rdon2",
        };
      } else if (type.compare("intron") == 0) {
        exons = std::vector<std::string> {
          "I0", "I1", "I2",
          "Is0", "If0", "Is1",
          "If1",  "Is2", "If2",
          "rI0", "rI1", "rI2",
          "rIs0", "rIf0", "rIs1",
          "rIf1", "rIs2", "rIf2",
        };
      }
      return exons;
    }

};

class ExtrinsicConverterAugustus: public ExtrinsicConverter{
  private:
    json _extrinsic_probabilities_contribuition;
    void addContribution(const size_t start, const size_t end,
                const string bonus_type, const string type){
    
    std::vector<std::string> exons =
     selectStatesToReceiveContribution(type);
    for (size_t j = start - 1; j < end; j++) {
      for (const auto& exon : exons) {
        if(double(_extrinsic_probabilities[_states_indices[exon]][j]) == double(1)){//only receive 1 hint by positon and by type
          //auto d = _extrinsic_probabilities[_states_indices[exon]][j];
          tops::model::Probability ec = double(_extrinsic_probabilities_contribuition[bonus_type][type]);
          _extrinsic_probabilities[_states_indices[exon]][j] =  ec;
        }     
      }
    }
  }
  
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
  
  tops::model::Matrix convertGtfLineToProbabilities(
    tops::model::Matrix &probabilities,
    const vector<GtfLine> &gtf_line,
    const string extrinsic_config) {
    
    _extrinsic_probabilities = probabilities;
    
    std::ifstream ifs(extrinsic_config);
    _extrinsic_probabilities_contribuition = json::parse(ifs);

    for (size_t i = 0; i < gtf_line.size(); i++) {
      GtfLine hl =  gtf_line.at(i);
      string type = hl._feature;
      int start = stoi(hl._start);
      int end = stoi(hl._end);
      if (type.compare("ep") == 0) {
          addContribution(start, end,  "bonus", "ep");
      } else if (type.compare("exon") == 0) {
          addContribution(start, end, "bonus", "exon");
      } else if (type.compare("start") == 0) {
          addContribution(start, end, "bonus", "start");
      } else if (type.compare("stop") == 0) {
          addContribution(start, end, "bonus", "stop");
      } else if (type.compare("ass") == 0) {
          addContribution(start, end, "bonus", "ass");
      } else if (type.compare("dss") == 0) {
          addContribution(start, end, "bonus", "dss");
      } else if (type.compare("intron") == 0) {
          addContribution(start, end, "bonus", "intron");
      }
      
    }
    
    //punishment for noHints
    for(size_t i = 0; i < _extrinsic_probabilities.size(); i++){
      for(size_t j = 0; j < _extrinsic_probabilities[i].size(); j++){
        if(double(_extrinsic_probabilities[i][j]) == double(1)){ //dont received hint contribution
          tops::model::Probability ec = double(_extrinsic_probabilities_contribuition["bonus"]["no_hints"]); 
          _extrinsic_probabilities[i][j] = ec; 
        }
      }
    }

    //printAllHints();
    return _extrinsic_probabilities;
  }

};

class ExtrinsicConverterTwinscan: public ExtrinsicConverter{
  private:
    VLMCPtr intron_vlmc;
    VLMCPtr cds_vlmc = trainVLCM(4, 5);
    VLMCPtr intergenic_vlmc;

  public:
    VLMCPtr trainVLCM(unsigned int alphabet_size,
                      unsigned int order) {
      auto vlmc_trainer = VLMC::standardTrainer();
      tops::model::Sequences sequences =  {
        {1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,0,0,1,1,1,1,0,1,1,0,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,0,1,1,1,1,1,1,},
        {1,0,0,1,0,0,0,1,1,0,0,1,1,0,0,1,1,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,},
        {1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,0,0,0,1,0,0,0,1,1,0,1,1,0,1,1,1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,0,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1,1,1,},
        {1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,1,0,0,1,0,1,1,1,1,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,0,0,1,0,0,1,1,0,0,1,1,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,1,0,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,0,1,1,1,1,1,1,1,0,0,1,1,1,0,1,1,1,1,0,1,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,0,1,0,0,1,1,0,1,1,1,0,1,0,1,1,0,1,1,0,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,0,1,1,0,0,1,1,1,1,1,0,0,1,0,0,1,1,1,1,0},
        {0,0,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,0,1,0,0,1,1,0,1,1,0,1,1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,0,1,1,0,0,1,1,0,1,1,0,0,1,1,1,1,0,1,1,0,0,1,1,1,0,0,1,1,0,1,1,0,1,0,0,1,1,0,1,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,0,0,0,1,1,0,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1},
        {1,0,1,0,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,0,1,0,1,1,0,0,1,1,0,0,1,1,0,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,0,1,0,1,1,1,1,0,0,0,0,0,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,0,0,1,0,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,0,1,1,0,0,1,1,1,1,0,1,1},
        {0,1,1,0,1,0,1,1,1,0,1,1,0,0,0,1,1,1,1,0,0,1,1,1,0,1,1,0,1,0,1,1,1,1,1,1,1,1,},
        {1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1,0,1,0,1,1,0,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,},
        {1,0,1,0,1,1,1,1,1,0,1,0,0,1,1,0,1,0,1,1,1,0,0,1,1,1,0,1,1,0,1,0,1,1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,0,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,0,1,0,0,1,1,0,1,1,0,},
        {1,1,1,1,1,1,0,0,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,0,0,1,1,0,1,1,1,0,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,0,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,0,1,1,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,},
        {1,0,1,0,0,1,1,0,1,1,1,1,1,0,1,1,0,0,1,1,1,1,0,1,1,0,1,1,0,1,1,0,0,0,1,1,1,0,1,1,0,1,0,1,0,0,0,1,1,1,1,0,1,0,0,0,1,1,0,0,1,1,0,1,0,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,0,0,1,0,1,1,1,0,1,1,0,1,1,1,1,0,0,0,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,1,0,1,1,0,1,0,0,0,1,1,1,1,1,1,0,0,1,0,0,1,1,0,0,1,0,1,1,1,1,0,0,0,1,1,0,1,1,1,1,0,1,0,0,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,0,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,0,0,1,0,0,0,1,1,1,0,1,1,1,0,1,0,},
        {1,0,1,1,1,0,1,0,0,1,0,1,0,0,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,0,1,0,},
        {0,1,0,1,1,0,1,1,0,1,1,0,0,1,0,1,1,1,1,0,0,1,0,1,1,0,1,1,0,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1,1,0,1,0,0,0,1,1,0,0,1,0,1,0,0,1,1,1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,0,0,1,0,1,0,0,0,1,0,1,1,0,0,1,1,1,0,0,0,1,1,1,0,1,1,0,1,0,1,1,0,1,0,1,1,1,0,0,0,1,0,0,0,0,1,0,1,1,0,1,0,1,0,0,0,0,0,1,0,0,1,1,1,1,0,0,1,1,0,1,0,0,0,1,1,0,1,0,0,1,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,0,1,0,0,1,1,0,0,0,1,1,0,1,0,1,1,0,1,1,0,0,1,1,1,0,1,0,1,1,1,0,0,0,1,1,0,1,1,1,1,1,1,0,1,1,1,1,0,0,1,0,1,1,1,1,1,0,0,1,0,1,0,0,0,1,0,1,0,1,1,0,1,1,1,1,1,0,},
        {0,0,0,1,0,1,0,0,0,0,1,1,0,1,0,0,1,0,1,1,1,0,1,0,0,1,0,0,1,1,0,1,1,0,1,},
        {1,0,1,0,1,1,1,0,1,0,0,1,1,0,1,1,0,1,1,0,0,0,0,1,0,1,1,1,0,1,1,0,1,1,0,1,0,1,1,0,0,1,0,1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,1,1,1,1,1,0,1,1,0,1,0,0,1,1,0,1,1,0,0,},
        {0,1,0,1,1,0,0,1,1,1,0,1,0,1,1,0,0,0,1,1,0,1,1,0,1,1,1,1,0,1,0,1,0,1,1,1,0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,1,1,0,0,0,1,0,1,1,0,0,0,0,0,0,1,1,0,1,1,1,0,0,1,0,1,0,0,0,0,1,0,1,0,0,1,0,0,1,0,1,1,1,0,0,1,},
        {0,0,1,1,0,0,0,1,0,0,1,1,1,0,0,0,1,1,1,1,0,1,0,0,0,1,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,1,0,1,0,0,0,1,0,1,0,1,0,},
        {0,1,0,1,1,1,1,0,1,1,1,0,0,1,0,0,1,0,0,0,0,1,1,0,1,1,1,1,0,1,0,0,1,1,1,0,1,0,1,0,1,1,1,0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,1,0,0,0,1,1,1,1,0,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,0,1,0,1,1,0,1,1,1,0,0,1,1,0,1,0,1,1,0,1,0,0,1,1,1,0,1,0,1,0,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,1,1,0,1,1,1,0,1,1,1,1,0,1,1,1,0,0,0,0,0,0,1,1,1,1,0,1,1,0,0,},
        {1,0,1,1,0,1,0,1,1,1,0,0,1,1,0,0,1,1,0,0,1,0,0,1,0,0,0,1,1,0,0,1,0,1,0,0,0,0,0,1,1,0,0,0,0,1,0,0,1,1,0,0,1,1,1,0,1,0,1,1,0,},
      };
    
      vlmc_trainer->add_training_set(sequences);
      auto vlmc = vlmc_trainer->train(
                      VLMC::fixed_length_algorithm{},
     //pseudocontador = 1.0, qdo n é informado,
     //pois Twinscan n informa nas suas CM de ordem 5
                      order, alphabet_size, 1.0,
                      std::vector<double>{1.0, 1.0, 1.0, 1.0},
                      nullptr);
  
      return vlmc;
    }

  VLMCPtr chooseTrainedVLCM(const std::string &type){
    if ((type.compare("ep") == 0) || (type.compare("exon") == 0)) {
      return cds_vlmc;
    }
    //TODO implementar os treinamentos das vlcms dos outros estados
    else{
      return cds_vlmc;
    }
  }

  tops::model::Matrix alterMatrixLines(const tops::model::Sequence
                                          &target_sequence,
                                    tops::model::Matrix &probabilities,
                                    const std::string &type){
    auto vlmc = chooseTrainedVLCM(type);
    auto evaluator = vlmc->standardEvaluator(tops::myop::conservation_sequence, true);
    //return probability sequence 0 to end
    evaluator->evaluateSequence(0,target_sequence.size());
    auto[ prefix_sum_array ] = std::dynamic_pointer_cast<
                                    tops::model::CachedEvaluator<
                                    tops::model::Standard, VLMC>>
                                    (evaluator)->cache();
    
    std::vector<std::string> exons =
      selectStatesToReceiveContribution(type);
    
    for (const auto& exon : exons) {
      probabilities[_states_indices[exon]] = prefix_sum_array;
    }

    return probabilities;
  }
};

//treinar com  fixed_length_algorithm, unsigned int order = 5,

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
    //for (const auto& p: prefix_sum_array){
      //std::cout << p << ' '; 
   // }

    //std::cout << std::endl <<"prob seq = " << target_sequence_probability << std::endl;
    
*/
