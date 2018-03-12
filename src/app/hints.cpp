#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
using namespace std;

//The HintLine has a line of gff hint file
class HintsLine{
  public:
    string sequenceName;
    string source;
    string feature;
    string start;
    string end;
    string score;
    string strand;
    string frame;
    string atribute;

    HintsLine(){
      this->sequenceName = "";
      this->source = "";
      this->feature = "";
      this->start = "";
      this->end = "";
      this->score = "";
      this->strand = "";
      this->frame = "";
      this->atribute = "";
    }

    HintsLine(string sequenceName, string source, string feature, string start, string end, string score,
     string strand, string frame, string atribute) {
      this->sequenceName = sequenceName;
      this->source = source;
      this->feature = feature;
      this->start = start;
      this->end = end;
      this->score = score;
      this->strand =strand;
      this->frame = frame;
      this->atribute = atribute;
    }

    void print_hint(){
      cout << this->sequenceName << '\t';
      cout << this->source << '\t';
      cout << this->feature << '\t';
      cout << this->start << '\t';
      cout << this->end << '\t';
      cout << this->score << '\t';
      cout << this->frame << '\t';
      cout << this->atribute << '\n';
      }
};

//The HintPoint class has the hints of a specific position of a sequence"
class HintPoint{
  public:
    unsigned short int start;
    unsigned short int stop;
    unsigned short int ep;
    unsigned short int exon;
    unsigned short int ass;
    unsigned short int dss;
    unsigned short int intron;
    unsigned short int null_type;
    string sequenceName;
    int position;
    HintPoint(string sequenceName, int position){
      start = 0;
      stop = 0;
      ep = 0;
      exon = 0;
      ass = 0;
      dss = 0;
      intron = 0;
      null_type = 0;
      this->sequenceName = sequenceName;
      this->position = position;
    }

    void print_hint(){
      cout << "name_seq: " << sequenceName << '\t';
      cout << "pos: " << position << '\t';
      cout << "start: " << start << '\t';
      cout << "stop: " << stop << '\t';
      cout << "ep: " << ep << '\t';
      cout << "exon: " << exon << '\t';
      cout << "ass: " << ass << '\t';
      cout << "dss: "<< dss << '\t';
      cout << "intron: "<< intron << '\t';
      cout << "null_type: "<< null_type << '\n';
    }

    void setType(string type){
      if (type.compare("start") == 0){
        start = 1;
      }
      else if (type.compare("stop") == 0){
        stop = 1;
      }
      else if (type.compare("ep") == 0){
        ep = 1;
      }
      else if (type.compare("exon") == 0){
        exon = 1;
      }
      else if (type.compare("ass") == 0){
        ass = 1;
      }
      else if (type.compare("dss") == 0){
        dss = 1;
      }
      else if (type.compare("intron") == 0){
        intron = 1;
      }
    }

    bool hintIsEmpty(){
      if (start == 0 and stop == 0 and ep == 0 and exon == 0 and ass == 0 and dss == 0){
        return true;
      }
      else{
        return false;
      }
    }

    void setAsNullHint(){
      null_type = 1;
    }
};

class FastaSequence{
  private:
  string sequenceName;
  string sequenceValue;
 
  public:
  FastaSequence(string sequenceName, string sequenceValue){
    this->sequenceName = sequenceName;
    this->sequenceValue = sequenceValue;
  }

  FastaSequence(){
  }

  void setSequenceName(string sequenceName){
    this->sequenceName = sequenceName;
  }

  void setSequenceValue(string sequenceValue){
    this->sequenceValue = sequenceValue; 
  }
  
  string getSequenceName(){
    return this->sequenceName;
  }

  string getSequenceValue(){
    return this->sequenceValue; 
  }
};

//based on Rosetta http://rosettacode.org/wiki/FASTA_format#C%2B%2B
class FastaConverter{
  public:
    vector<FastaSequence> converteFastaFileToFastaSequence(string fastaFile){
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


};

//The Hints class has 3 columns (feature, start and end) of all hints in gff file.
class Hints{
  public:

  int numberOfSequences;
  vector<string> sequencesNames;
  vector<vector<HintPoint>> allHints;

  Hints(vector<FastaSequence> fastaSequences, vector<HintsLine> hintsLine){
    
    for(size_t i = 0; i < fastaSequences.size(); i++){
      vector<HintPoint> aux;
      unsigned int sequenceLenght = fastaSequences.at(i).getSequenceValue().length();
      string sequence_name = fastaSequences.at(i).getSequenceName();
      
      for(size_t j = 0; j < sequenceLenght; j++){
        aux.push_back(HintPoint(sequence_name, j));
      }
      allHints.push_back(aux);
    }
  }

  void setAllEmptyHintsAsNullHints(vector<FastaSequence> fs){
    for(size_t i = 0; i < fs.size(); i++){
      for(size_t j = 0; j < fs.at(i).getSequenceValue().length(); j++){
        if(allHints.at(i).at(j).hintIsEmpty()){
          allHints.at(i).at(j).setAsNullHint();
        }
      }
    }
  }

  void printAllHints(vector<FastaSequence> fs){
    for(size_t i = 0; i < fs.size(); i++){
      for(size_t j = 0; j < fs.at(i).getSequenceValue().length(); j++){
        allHints.at(i).at(j).print_hint();
      }
    }
  }

  vector<string> getSequencesNames(vector<HintsLine> hintsLine){
    unordered_map<string,string> sequencesNamesHash;
    vector<string> sequencesNames;
    for(size_t i = 0; i < hintsLine.size(); i++){
      string sequenceName = hintsLine.at(i).sequenceName;
      sequencesNamesHash.emplace(sequenceName, sequenceName);
    }
    unordered_map<string, string>::iterator it = sequencesNamesHash.begin();
    while (it != sequencesNamesHash.end()){
        sequencesNames.push_back(it->first);
		  it++;
    }
    return sequencesNames;
  }

  int getNumberOfSequencesNames(vector<HintsLine> hintsLine){
    unordered_map<string,string> sequencesNamesHash;
    for(size_t i = 0; i < hintsLine.size(); i++){
      string sequenceName = hintsLine.at(i).sequenceName;
      sequencesNamesHash.emplace(sequenceName, sequenceName);
    }
    return sequencesNamesHash.size();
  }
};

class HintsConverter{
  public:
  
  vector<HintsLine> convertGffFileToHintsLine(string fileName){
    ifstream hintsFile(fileName);
    string line;
    vector<string> tokens;
    vector<HintsLine>  hintsLine;

    while(getline(hintsFile, line)) {
      istringstream iss(line);
      string token;
      while(getline(iss, token, '\t'))
          tokens.push_back(token);
    }

    for (size_t i = 0; i < tokens.size(); i+=9){ //+9 to jump to next line of gff file
      HintsLine n = HintsLine(tokens[i], tokens[i+1], tokens[i+2], tokens[i+3], tokens[i+4], tokens[i+5], tokens[i+6], tokens[i+7], tokens[i+8]);
      hintsLine.push_back(n);
    }
    return hintsLine;
  }

  Hints *convertHintsLineToHints(vector<FastaSequence> fastaSequences, vector<HintsLine> hintsLine){
    Hints *hints = new Hints(fastaSequences, hintsLine);
    
    for(size_t i = 0; i < hintsLine.size(); i++){
      HintsLine hl =  hintsLine.at(i);
      string type = hl.feature;
      string sequenceName = hl.sequenceName;
      int start = stoi(hl.start);
      int end = stoi(hl.end);
      
      for(size_t j = 0; j < hints->allHints.size(); j++){
        int sizeHintsSequence = hints->allHints.at(j).size();
        string name_hints = hints->allHints.at(j).at(0).sequenceName;

        if(sequenceName.compare(name_hints) == 0){
          for(size_t k = start; k <=end; k++){
            hints->allHints.at(j).at(k).setType(type);
          }
        }
      }
    }
      
    return hints;
  }

};

int main(int argc, char const *argv[]) {
  
  FastaConverter *fc = new FastaConverter();
  vector<FastaSequence> fastaSequences = fc->converteFastaFileToFastaSequence("2-seq-treinamento.fasta");
  
  HintsConverter *hc = new HintsConverter();
  vector<HintsLine> hintsLine = hc->convertGffFileToHintsLine("2-seq-hints.gff");
  Hints *hints = hc->convertHintsLineToHints(fastaSequences, hintsLine);
  
  hints->setAllEmptyHintsAsNullHints(fastaSequences);
  hints->printAllHints(fastaSequences);

  return 0;
}
