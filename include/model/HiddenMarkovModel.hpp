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

#ifndef TOPS_MODEL_HIDDEN_MARKOV_MODEL_
#define TOPS_MODEL_HIDDEN_MARKOV_MODEL_

#include <memory>

#include "src/HiddenMarkovModel.hpp"

namespace tops {
namespace model {

class HiddenMarkovModel;
typedef std::shared_ptr<HiddenMarkovModel> HiddenMarkovModelPtr;

class HiddenMarkovModel : public tops::HiddenMarkovModel {
 public:
  static HiddenMarkovModelPtr make(std::vector<tops::HMMStatePtr> states, DiscreteIIDModelPtr initial_probability, int observation_alphabet_size, int state_alphabet_size);
 private:
  HiddenMarkovModel(std::vector<tops::HMMStatePtr> states, DiscreteIIDModelPtr initial_probability, int observation_alphabet_size, int state_alphabet_size);
};

}
}

#endif  // TOPS_MODEL_HIDDEN_MARKOV_MODEL_
