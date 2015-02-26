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

#ifndef TOPS_MODEL_HIDDEN_MARKOV_MODEL_STATE_
#define TOPS_MODEL_HIDDEN_MARKOV_MODEL_STATE_

// Standard headers
#include <memory>
#include <vector>

// ToPS headers
#include "DiscreteIIDModel.hpp"

namespace tops {
namespace model {

class HiddenMarkovModelState;

/**
 * @typedef HiddenMarkovModelStatePtr
 * @brief Alias of pointer to HiddenMarkovModelState.
 */
using HiddenMarkovModelStatePtr = std::shared_ptr<HiddenMarkovModelState>;

/**
 * @class HiddenMarkovModelState
 * @brief TODO
 */
class HiddenMarkovModelState {
 public:
  HiddenMarkovModelStatePtr make(Symbol symbol, DiscreteIIDModelPtr emissions, DiscreteIIDModelPtr transitions);
  DiscreteIIDModelPtr & emissions();
  DiscreteIIDModelPtr & transitions();
  bool isSilent();
  int symbol();
 private:
  HiddenMarkovModelState(Symbol symbol, DiscreteIIDModelPtr emissions, DiscreteIIDModelPtr transitions);

  Symbol _symbol;
  DiscreteIIDModelPtr _emissions;
  DiscreteIIDModelPtr _transitions;
};

typedef std::vector<HiddenMarkovModelState> HiddenMarkovModelStateSequence;

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_HIDDEN_MARKOV_MODEL_STATE_
