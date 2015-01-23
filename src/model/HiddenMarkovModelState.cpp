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

#include "HiddenMarkovModelState.hpp"

namespace tops {
namespace model {

HiddenMarkovModelState::HiddenMarkovModelState(Symbol symbol, DiscreteIIDModelPtr emission, DiscreteIIDModelPtr transition) {
  _self = tops::HMMStatePtr(new tops::HMMState());
  _self->setEmissions(boost::static_pointer_cast<tops::DiscreteIIDModel>(emission->_self));
  _self->setTransition(boost::static_pointer_cast<tops::DiscreteIIDModel>(transition->_self));
  _self->setId(symbol);
}

DiscreteIIDModelPtr HiddenMarkovModelState::emissions() {
  auto iid = DiscreteIIDModelPtr(new DiscreteIIDModel({}));
  iid->_self = _self->emission();
  return iid;
}

DiscreteIIDModelPtr HiddenMarkovModelState::transitions() {
  auto iid = DiscreteIIDModelPtr(new DiscreteIIDModel({}));
  iid->_self = _self->transitions();
  return iid;
}

}  // namespace model
}  // namespace tops
