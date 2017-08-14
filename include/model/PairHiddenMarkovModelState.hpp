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

#ifndef TOPS_MODEL_PAIR_HIDDEN_MARKOV_MODEL_STATE_
#define TOPS_MODEL_PAIR_HIDDEN_MARKOV_MODEL_STATE_

// Standard headers
#include <memory>
#include <vector>

namespace tops {
namespace model {

// Forward declarations
class PairHiddenMarkovModel;

class DiscreteIIDModel;
using IID = DiscreteIIDModel;

template<typename Model> struct StateTraits;
template<typename EmissionModel, typename TransitionModel> class State;
template<typename EmissionModel, typename TransitionModel,
         std::size_t dimension, bool... gaps> class SimpleState;

/**
 * @typedef StateTraits
 * @brief PairHiddenMarkovModel specialization of StateTraits
 */
template<>
struct StateTraits<PairHiddenMarkovModel> {
  using State = tops::model::State<IID, IID>;

  using MatchState  = SimpleState<IID, IID, 2, false, false >;
  using Gap1State   = SimpleState<IID, IID, 2, true,  false >;
  using Gap2State   = SimpleState<IID, IID, 2, false, true  >;
  using SilentState = SimpleState<IID, IID, 2, true,  true  >;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_PAIR_HIDDEN_MARKOV_MODEL_STATE_
