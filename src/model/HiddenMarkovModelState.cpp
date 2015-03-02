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

// Standard headers
#include <cmath>
#include <vector>
#include <algorithm>

// ToPS headers
#include "HiddenMarkovModelState.hpp"

namespace tops {
namespace model {

HiddenMarkovModelStatePtr HiddenMarkovModelState::make(
    Symbol symbol,
    DiscreteIIDModelPtr emissions,
    DiscreteIIDModelPtr transitions) {
  return HiddenMarkovModelStatePtr(new HiddenMarkovModelState(
      symbol, emissions, transitions));
}

HiddenMarkovModelState::HiddenMarkovModelState(
    Symbol symbol,
    DiscreteIIDModelPtr emissions,
    DiscreteIIDModelPtr transitions)
    : _symbol(symbol),
      _emissions(emissions),
      _transitions(transitions) {
}

DiscreteIIDModelPtr & HiddenMarkovModelState::emissions() {
  return _emissions;
}

DiscreteIIDModelPtr & HiddenMarkovModelState::transitions() {
  return _transitions;
}

bool HiddenMarkovModelState::isSilent() {
  return (_emissions == NULL);
}

int HiddenMarkovModelState::symbol() {
  return _symbol;
}


}  // namespace model
}  // namespace tops