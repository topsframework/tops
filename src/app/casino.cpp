// Standard headers
#include <vector>
#include <string>
#include <iostream>

// Internal headers
#include "model/Sequence.hpp"
#include "model/Probability.hpp"

#include "model/Labeler.hpp"
#include "model/GeometricDuration.hpp"

#include "model/DiscreteIIDModel.hpp"
#include "model/GeneralizedHiddenMarkovModel.hpp"

// Imports
using tops::model::Labeler;
using tops::model::Sequence;
using tops::model::Probability;
using tops::model::GeometricDuration;

// Aliases
using IID = tops::model::DiscreteIIDModel;
using GHMM = tops::model::GeneralizedHiddenMarkovModel;

// Auxiliary functions
std::vector<Probability> index_probabilities(const std::size_t num_states,
                                             const std::vector<GHMM::State::Id>& indices,
                                             const std::vector<Probability>& probabilities) {
  std::vector<Probability> indexed_probabilities(num_states);

  for (std::size_t i = 0; i < indices.size(); i++)
    indexed_probabilities[indices[i]] = probabilities[i];

  return indexed_probabilities;
}

void connect(GHMM::StatePtr predecessor, GHMM::StatePtr successor){
    predecessor->addSuccessor(successor->id());
    successor->addPredecessor(predecessor->id());
}

int main() {
  const auto casino_num_states = 2;
  const auto dice_num_sides = 6;

  // IDs
  auto fair_id = 0;
  auto loaded_id = 1;

  // Initial probabilities
  auto initial_probabilities
          = IID::make(std::vector<Probability>{ 0.5, 0.5 });

  // Emissions
  auto fair_emissions
          = IID::make(std::vector<Probability>{
                  1.0/6, 1.0/6, 1.0/6, 1.0/6, 1.0/6, 1.0/6 });

  auto loaded_emissions
          = IID::make(std::vector<Probability>{
                  0.5, 0.1, 0.1, 0.1, 0.1, 0.1});

  // Transitions
  auto fair_transition_probabilities
          = index_probabilities(casino_num_states, { 0, 1 }, { 0.9, 0.1 });
  auto fair_transitions
          = IID::make(fair_transition_probabilities);

  auto loaded_transition_probabilities
          = index_probabilities(casino_num_states, { 0, 1 }, { 0.1, 0.9 });
  auto loaded_transitions
          = IID::make(loaded_transition_probabilities);

  // Durations
  auto fair_durations
          = GeometricDuration::make(fair_id, fair_transitions);

  auto loaded_durations
          = GeometricDuration::make(loaded_id, loaded_transitions);

  // State definitions
  auto fair_state
    = GHMM::State::make(fair_id, fair_emissions, fair_transitions, fair_durations);

  auto loaded_state
    = GHMM::State::make(loaded_id, loaded_emissions, loaded_transitions, loaded_durations);

  // GHMM definition
  std::vector<GHMM::StatePtr> ghmm_states { fair_state, loaded_state };

  auto ghmm = GHMM::make(
      ghmm_states, initial_probabilities, casino_num_states, dice_num_sides);

  connect(loaded_state, loaded_state);
  connect(loaded_state, fair_state);
  connect(fair_state, fair_state);
  connect(fair_state, loaded_state);

  // Test 1
  {
    Sequence observation {
            3, 0, 0, 0, 4, 1, 1, 0, 1, 4, 3, 4, 5, 3, 1, 5, 1, 3, 5, 0,
            3, 0, 0, 0, 0, 3, 0, 0, 0, 0, 3, 3, 5, 2, 0, 1, 3, 0, 1, 2,
            4, 1, 4, 1, 1, 0, 0, 5, 1, 5, 2, 2, 1, 0, 1, 3, 1, 1, 0, 2,
            1, 0, 2, 2, 3, 5, 2, 0, 0, 3, 0, 1, 0, 0, 0, 2, 0, 5, 3, 0,
            3, 1, 2, 1, 5, 5, 2, 2, 2, 0, 2, 0, 1, 0, 3, 2, 2, 4, 2, 3
    };

    auto labeler = ghmm->labeler(observation, true);
    auto estimation = labeler->labeling(Labeler::method::bestPath);
    auto labeling = estimation.estimated();

    for (auto symbol : labeling.label())
      std::cout << symbol;
    std::cout << std::endl;
  }

  std::cout << "-----" << std::endl;

  // Test 2
  {
    Sequence observation {
            1, 3, 4, 3, 3, 0, 4, 2, 5, 4, 0, 3, 1, 0, 3, 2, 2, 1, 3, 2,
            0, 4, 4, 5, 1, 4, 5, 0, 4, 3, 1, 0, 5, 0, 0, 3, 0, 0, 4, 3,
            4, 1, 4, 0, 2, 5, 0, 3, 0, 1, 0, 4, 4, 0, 5, 2, 0, 3, 4, 4,
            1, 2, 4, 1, 5, 5, 0, 4, 1, 3, 5, 2, 3, 1, 2, 4, 3, 5, 1, 3,
            5, 1, 0, 0, 5, 0, 0, 0, 5, 4, 0, 0, 2, 0, 0, 4, 1, 0, 0, 0,
    };

    auto labeler = ghmm->labeler(observation, true);
    auto estimation = labeler->labeling(Labeler::method::bestPath);
    auto labeling = estimation.estimated();

    for (auto symbol : labeling.label())
      std::cout << symbol;
    std::cout << std::endl;
  }

  return 0;
}
