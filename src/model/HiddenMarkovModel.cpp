/***********************************************************************/
/*  Copyright 2015 ToPS                                                */
/*                                                                     */
/*  This program is free software; you can redistribute it and/or      */
/*  modify it under the terms of the GNU  General Public License as    */
/*  published by the Free Software Foundation; either version 3 of     */
/*  the License, or (at your option) any later version.                */
/*                                                                     */
/*  This program is distributed in the hope that it will be useful,    */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of     */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      */
/*  GNU General Public License for more details.                       */
/*                                                                     */
/*  You should have received a copy of the GNU General Public License  */
/*  along with this program; if not, write to the Free Software        */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,         */
/*  MA 02110-1301, USA.                                                */
/***********************************************************************/

#include "HiddenMarkovModel.hpp"

namespace tops {
namespace model {

HiddenMarkovModelPtr HiddenMarkovModel::make(std::vector<tops::HMMStatePtr> states, DiscreteIIDModelPtr initial_probability, int observation_alphabet_size, int state_alphabet_size) {
  return HiddenMarkovModelPtr(new HiddenMarkovModel(states, initial_probability, observation_alphabet_size, state_alphabet_size));
}

HiddenMarkovModel::HiddenMarkovModel(std::vector<tops::HMMStatePtr> states, DiscreteIIDModelPtr initial_probability, int observation_alphabet_size, int state_alphabet_size) {
  auto states_alphabet = AlphabetPtr(new Alphabet());
  for (int i = 0; i < state_alphabet_size; i++)
    states_alphabet->createSymbol(std::to_string(i));

  auto observation_alphabet = AlphabetPtr(new Alphabet());
  for (int i = 0; i < observation_alphabet_size; i++)
    observation_alphabet->createSymbol(std::to_string(i));

  setStates(states, states_alphabet);
  setAlphabet(observation_alphabet);
  setInitialProbability(initial_probability);
}

}
}
