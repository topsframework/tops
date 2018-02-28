#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class HintLine{
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

    HintLine(){
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

    HintLine(string name_sequence, string source, string feature, string start, string end, string score,
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

class Hints{
  public:
  vector<HintPoint*> *hints;
   
  Hints(int max_sequence_length){
    hints = new vector<HintPoint*>();
    for(int i = 0; i < max_sequence_length; i++){
      hints->push_back(new HintPoint());
    }
  }

  void setAllEmptyHintsAsNullHints(){
    for(int i = 0; i < hints->size(); i++){
      if(hints->at(i)->hintIsEmpty()){
        hints->at(i)->setAsNullHint();
      }
    }
  }

  void printAllHints(){
    for(int i = 0; i < hints->size(); i++){
      hints->at(i)->print_hint();
    }
  }

  bool allHintsHasAtype(){
    for(int i = 0; i < hints->size(); i++){
      
    }


  }
};

int main () { 

std::ifstream file("hints.gff");

std::string line;
std::vector<std::string> tokens;
vector<HintLine*> hintsLine;

while(std::getline(file, line)) {     // '\n' is the default delimiter
    std::istringstream iss(line);
    std::string token;
    while(std::getline(iss, token, '\t'))   // but we can specify a different one
        tokens.push_back(token);
}

for (std::size_t i = 0; i < tokens.size(); i+=9){
  HintLine *n = new HintLine(tokens[i], tokens[i+1], tokens[i+2], tokens[i+3], tokens[i+4], tokens[i+5], tokens[i+6], tokens[i+7], tokens[i+8]);
  hintsLine.push_back(n);
}

Hints *hints = new Hints(5000);

for(std::size_t i = 0; i < hintsLine.size(); i++){
  HintLine *hl = new HintLine();
  hl =  hintsLine[i];
  string type = hl->feature;
  int start = stoi(hl->start);
  int stop = stoi(hl->end);
  for(int i = start-1; i < stop; i++){ //-1 to sincronize vector e hints
    hints->hints->at(i)->setType(type);
  }
}

hints->setAllEmptyHintsAsNullHints();
hints->printAllHints();

return 0;
}