#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

//The HintLine has a line of gff hint file
class HintsLine{
  public:
    string name_sequence;
    string source;
    string feature;
    string start;
    string end;
    string score;
    string strand;
    string frame;
    string atribute;

    HintsLine(){
      this->name_sequence = "";
      this->source = "";
      this->feature = "";
      this->start = "";
      this->end = "";
      this->score = "";
      this->strand = "";
      this->frame = "";
      this->atribute = "";

    }

    HintsLine(string name_sequence, string source, string feature, string start, string end, string score,
     string strand, string frame, string atribute) {
      this->name_sequence = name_sequence;
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
      cout << this->name_sequence << '\t';
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
    HintPoint(){
      start = 0;
      stop = 0;
      ep = 0;
      exon = 0;
      ass = 0;
      dss = 0;
      intron = 0;
      null_type = 0;
    }

    HintPoint(unsigned short int start, unsigned short int stop, unsigned short int ep, unsigned short int exon,
     unsigned short int ass, unsigned short int dss, unsigned short int intron, unsigned short int null_type){
      this->start = start;
      this->stop = stop;
      this->ep = ep;
      this->exon = exon;
      this->ass = ass;
      this->dss = dss;
      this->intron = intron;
      this->null_type = null_type;
    }

    void print_hint(){
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

//"The Hints class has 3 columns (feature, start and end) of all hints in gff file.
class Hints{
  public:
  vector<HintPoint*> *hints;

  Hints(int max_sequence_length){
    hints = new vector<HintPoint*>();
    for(size_t i = 0; i < max_sequence_length; i++){
      hints->push_back(new HintPoint());
    }
  }

  void setAllEmptyHintsAsNullHints(){
    for(size_t i = 0; i < hints->size(); i++){
      if(hints->at(i)->hintIsEmpty()){
        hints->at(i)->setAsNullHint();
      }
    }
  }

  void printAllHints(){
    for(size_t i = 0; i < hints->size(); i++){
      hints->at(i)->print_hint();
    }
  }

};

class HintsConverter{
  public:
  vector<HintsLine*> *hintsLine;
  Hints *hints = new Hints(5000);

  vector<HintsLine*> *convertGffFileToHintsLine(string fileName){
    hintsLine = new vector<HintsLine*>();
    ifstream hintsFile(fileName);
    string line;
    vector<string> tokens;

    while(getline(hintsFile, line)) {
      istringstream iss(line);
      string token;
      while(getline(iss, token, '\t'))
          tokens.push_back(token);
    }

    for (size_t i = 0; i < tokens.size(); i+=9){ //+9 to jump to next line of gff file
      HintsLine *n = new HintsLine(tokens[i], tokens[i+1], tokens[i+2], tokens[i+3], tokens[i+4], tokens[i+5], tokens[i+6], tokens[i+7], tokens[i+8]);
      hintsLine->push_back(n);
    }
    return hintsLine;
  }

  Hints *convertHintsLineToHints(vector<HintsLine*> *hintsLine){
    for(size_t i = 0; i < hintsLine->size(); i++){
      HintsLine *hl = new HintsLine();
      hl =  hintsLine->at(i);
      string type = hl->feature;
      int start = stoi(hl->start);
      int stop = stoi(hl->end);
      for(size_t i = start-1; i < stop; i++){ //-1 to sincronize vector indices and hints indices
        hints->hints->at(i)->setType(type);
      } 
    }
    return hints;
  }
};

int main(int argc, char const *argv[]) {

HintsConverter *hc = new HintsConverter();
vector<HintsLine*> *hintsLine = new vector<HintsLine*>();
hintsLine = hc->convertGffFileToHintsLine("hints.gff");
Hints *hints = new Hints(5000);
hints = hc->convertHintsLineToHints(hintsLine);
hints->setAllEmptyHintsAsNullHints();
hints->printAllHints();

return 0;
}

