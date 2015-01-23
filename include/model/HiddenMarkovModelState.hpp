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

#ifndef HIDDEN_MARKOV_MODEL_STATE_HPP_
#define HIDDEN_MARKOV_MODEL_STATE_HPP_

#include <memory>

#include "DiscreteIIDModel.hpp"

#include "src/HiddenMarkovModel.hpp"

namespace tops {
namespace model {

class HiddenMarkovModelState {
 public:
  HiddenMarkovModelState(Symbol symbol, DiscreteIIDModelPtr emission, DiscreteIIDModelPtr transition);

  DiscreteIIDModelPtr emissions();
  DiscreteIIDModelPtr transitions();

  tops::HMMStatePtr _self;
};

typedef std::shared_ptr<HiddenMarkovModelState> HiddenMarkovModelStatePtr;

}  // namespace model
}  // namespace tops

#endif
