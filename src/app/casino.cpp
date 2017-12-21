#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <dirent.h>

#include "model/DiscreteIIDModel.hpp"
#include "model/VariableLengthMarkovChain.hpp"
#include "model/InhomogeneousMarkovChain.hpp"
#include "model/PeriodicInhomogeneousMarkovChain.hpp"
#include "model/MultipleSequentialModel.hpp"
#include "model/MaximalDependenceDecomposition.hpp"

//Novos includes
#include "model/Util.hpp"
#include "model/Matrix.hpp"
#include "model/Sequence.hpp"
#include "model/Probability.hpp"
#include "model/SignalDuration.hpp"
#include "model/ExplicitDuration.hpp"
#include "model/GeometricDuration.hpp"
#include "exception/NotYetImplemented.hpp"
#include "helper/Sequence.hpp"
#include "helper/SExprTranslator.hpp"
#include "model/GeneralizedHiddenMarkovModel.hpp"
#include "model/VariableLengthMarkovChain.hpp"
#include "helper/VariableLengthMarkovChain.hpp"
#include "helper/DiscreteIIDModel.hpp"
#include "model/Standard.hpp"

using tops::model::Sequence;
using tops::model::ProbabilisticModelPtr;
using tops::model::DiscreteIIDModel;
using tops::model::DiscreteIIDModelPtr;
using tops::model::VariableLengthMarkovChain;
using tops::model::VariableLengthMarkovChainPtr;
using tops::model::InhomogeneousMarkovChain;
using tops::model::InhomogeneousMarkovChainPtr;
using tops::model::PeriodicInhomogeneousMarkovChain;
using tops::model::PeriodicInhomogeneousMarkovChainPtr;
using tops::model::MultipleSequentialModel;
using tops::model::MultipleSequentialModelPtr;
using tops::model::Probability;
using tops::model::ContextTree;
using tops::model::MaximalDependenceDecomposition;
using tops::model::Consensus;

//Novos namespaces
using tops::model::GeneralizedHiddenMarkovModel;
using tops::model::GeneralizedHiddenMarkovModelPtr;
using tops::helper::createVLMCMC;
using tops::model::SignalDuration;
using tops::helper::createFairCoinIIDModel;
using tops::model::ExplicitDuration;
using tops::helper::createMachlerVLMC;
using tops::model::GeometricDuration;
using tops::model::Matrix;
using tops::model::Standard;
using tops::model::EvaluatorPtr;
using tops::model::Labeler;
using tops::helper::generateRandomIIDModel;
using namespace std;


using Symbol = unsigned int;
using GHMM = GeneralizedHiddenMarkovModel;

std::vector<Probability> *cria_vetor_transicao(std::vector<unsigned int> vetor_indices,
  std::vector<Probability> &vetor_probabilidades){

   std::vector<Probability> *retorno = new vector<Probability>;

   for(int i = 0; i < 56; i++){
     retorno->push_back(0);
   }

   for(unsigned int i = 0; i < vetor_indices.size(); i++){
     retorno->erase(retorno->begin() + vetor_indices.at(i));
     retorno->insert(retorno->begin() + vetor_indices.at(i),
     vetor_probabilidades.at(i));
   }

   return retorno;
}

void connect(GHMM::StatePtr predecessor, GHMM::StatePtr successor){
    predecessor->addSuccessor(successor->id());
    successor->addPredecessor(predecessor->id());
}

int main() {

  //Transitions probabilities
  auto fair_probabilities = vector<Probability>{{0.9, 0.1}};
  auto fair_probabilities_indices = vector<unsigned int>{0, 1};
  auto indexed_fair_probabilities = cria_vetor_transicao(
    fair_probabilities_indices, fair_probabilities);

    DiscreteIIDModelPtr fair_geometric_transition
      = DiscreteIIDModel::make(*indexed_fair_probabilities);

  auto loaded_probabilities = vector<Probability>{{0.1, 0.9}};
  auto loaded_probabilities_indices = vector<unsigned int>{0, 1};
  auto indexed_loaded_probabilities = cria_vetor_transicao(
     loaded_probabilities_indices, loaded_probabilities);

     DiscreteIIDModelPtr loaded_geometric_transition
    = DiscreteIIDModel::make(*indexed_loaded_probabilities);

  //Emissions probabilities
  DiscreteIIDModelPtr fair_emission
    = DiscreteIIDModel::make(std::vector<Probability>{{
      0.166666666, 0.166666666, 0.166666666,
       0.166666666, 0.166666666, 0.166666666}});

  DiscreteIIDModelPtr loaded_emission
  = DiscreteIIDModel::make(std::vector<Probability>{{
     0.5, 0.1, 0.1,
      0.1, 0.1, 0.1}});

  //States definitions
  GHMM::StatePtr fair_state //um modelo qq
    = GHMM::State::make(
      0, fair_emission, //prob de emissão
      fair_geometric_transition, //prob de trans
      GeometricDuration::make(0, fair_geometric_transition)); //prob de duração geo

  GHMM::StatePtr loaded_state //um modelo qq
    = GHMM::State::make(
      1, loaded_emission, //prob de emissão
      loaded_geometric_transition, //prob de trans
      GeometricDuration::make(1
        , loaded_geometric_transition)); //prob de duração geo

  //junta tudo
  //GHMM definition
  GeneralizedHiddenMarkovModelPtr ghmm
    = GeneralizedHiddenMarkovModel::make(
      std::vector<GeneralizedHiddenMarkovModel::StatePtr>{
        fair_state, //estado 0
        loaded_state }, // estado 1
      DiscreteIIDModel::make(std::vector<Probability>{{ 0.5, 0.5}}), //dist de estados ini
      2, //numero de estados
       6); //cardinalidade do alfabeto de observacoes

  //GHMM graph conection
  connect(loaded_state, loaded_state);
  connect(loaded_state, fair_state);
  connect(fair_state, fair_state);
  connect(fair_state, loaded_state);

  Sequence observation {3, 0, 0, 0, 4, 1, 1, 0, 1, 4, 3, 4, 5, 3, 1, 5, 1, 3,
     5, 0, 3, 0, 0, 0, 0, 3, 0, 0, 0, 0, 3, 3, 5, 2, 0, 1, 3, 0, 1,
     2, 4, 1, 4, 1, 1, 0, 0, 5, 1, 5, 2, 2, 1, 0, 1, 3, 1, 1, 0, 2, 1, 0, 2,
     2, 3, 5, 2, 0, 0, 3, 0, 1, 0, 0, 0, 2, 0, 5, 3, 0, 3, 1, 2, 1, 5, 5, 2,
     2, 2, 0, 2, 0, 1, 0, 3, 2, 2, 4, 2, 3};


  auto labeler = ghmm->labeler(observation, true);
  auto estimation = labeler->labeling(Labeler::method::bestPath);
  auto labeling = estimation.estimated();

  for (auto symbol : labeling.label()) {
    std::cout << symbol;
  }
  std::cout << std::endl << "-----" << std::endl;

  observation =  {1, 3, 4, 3, 3, 0, 4, 2, 5, 4, 0, 3, 1, 0, 3, 2, 2, 1, 3, 2,
    0, 4, 4, 5, 1, 4, 5, 0, 4, 3, 1, 0, 5, 0, 0, 3, 0, 0, 4, 3, 4, 1, 4, 0, 2,
    5, 0, 3, 0, 1, 0, 4, 4, 0, 5, 2, 0, 3, 4, 4, 1, 2, 4, 1, 5, 5, 0, 4, 1, 3,
    5, 2, 3, 1, 2, 4, 3, 5, 1, 3, 5, 1, 0, 0, 5, 0, 0, 0, 5, 4, 0, 0, 2, 0, 0,
    4, 1, 0, 0, 0};

  labeler = ghmm->labeler(observation, true);
  estimation = labeler->labeling(Labeler::method::bestPath);
  labeling = estimation.estimated();

  for (auto symbol : labeling.label()) {
    std::cout << symbol;
  }
  std::cout << std::endl;

  std::vector<Probability> vetor_probabilidades;
  vetor_probabilidades = {1, 0.5, 0.8, 0.6};
  std::vector<unsigned int> vetor_indices;
  vetor_indices.push_back(0);
  vetor_indices.push_back(3);
  vetor_indices.push_back(5);
  vetor_indices.push_back(55);


  std::vector<Probability> *a = cria_vetor_transicao(vetor_indices,
  vetor_probabilidades);

  cout << a->size() << endl;
  cout << vetor_indices.size() << endl;
  cout << vetor_probabilidades.size() << endl;

  for(unsigned int i = 0; i < a->size(); i++){
    cout << a->at(i) << " ";
  }
  std::cout << std::endl;

  return 0;
}
