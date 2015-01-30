#include "model/HiddenMarkovModel.hpp"

#include "helper/DiscreteIIDModel.hpp"

#include <string>

namespace tops {
namespace helper {

tops::model::HiddenMarkovModelPtr generateRandomHMM(int number_of_states, int number_of_observations) {
  auto initial_probabilities = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(*generateRandomIIDModel(number_of_observations).get()));

  std::vector<tops::HMMStatePtr> states;
  auto state_alphabet = tops::AlphabetPtr(new tops::Alphabet());
  for (int i = 0; i < number_of_states; i++) {
    state_alphabet->createSymbol(std::to_string(i));
    auto s = tops::HMMStatePtr(new tops::HMMState());
    s->setEmissions(tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(*generateRandomIIDModel(number_of_observations).get())));
    s->setTransition(tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(*generateRandomIIDModel(number_of_states).get())));
    s->setName(state_alphabet->getSymbol(std::to_string(i)));
    states.push_back(s);
  }

  return tops::model::HiddenMarkovModel::make(states, initial_probabilities, number_of_observations, number_of_states);
}

tops::model::HiddenMarkovModelPtr createDishonestCoinCasinoHMM() {
  auto initial_probabilities = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(std::vector<double>{0.5, 0.5}));
  auto loaded_distribution = tops::DiscreteIIDModelPtr(new tops::DiscreteIIDModel(std::vector<double>{0.2, 0.8}));

  auto state_alphabet = tops::AlphabetPtr(new tops::Alphabet());
  state_alphabet->createSymbol("fair");
  state_alphabet->createSymbol("loaded");

  std::vector<tops::HMMStatePtr> states;

  auto fair_state = tops::HMMStatePtr(new tops::HMMState());
  fair_state->setEmissions(initial_probabilities);
  fair_state->setTransition(initial_probabilities);
  fair_state->setName(state_alphabet->getSymbol("fair"));
  states.push_back(fair_state);

  auto loaded_state = tops::HMMStatePtr(new tops::HMMState());
  loaded_state->setEmissions(loaded_distribution);
  loaded_state->setTransition(loaded_distribution);
  loaded_state->setName(state_alphabet->getSymbol("loaded"));
  states.push_back(loaded_state);

  return tops::model::HiddenMarkovModel::make(states, initial_probabilities, 2, 2);
}

}  // namespace helper
}  // namespace tops