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

#ifndef TOPS_MODEL_GENERALIZED_HIDDEN_MARKOV_MODEL_STATE_
#define TOPS_MODEL_GENERALIZED_HIDDEN_MARKOV_MODEL_STATE_

// Standard headers
#include <memory>
#include <vector>

namespace tops {
namespace model {

// Forward declarations
class ProbabilisticModel;
using Model = ProbabilisticModel;

class DiscreteIIDModel;
using IID = DiscreteIIDModel;

class GeneralizedHiddenMarkovModel;
using GHMM = GeneralizedHiddenMarkovModel;

template<typename Model> struct StateTraits;
template<typename EmissionModel, typename TransitionModel> class State;
template<typename EmissionModel, typename TransitionModel,
         size_t dimension, bool... gaps> class SimpleState;

/**
 * @typedef StateTraits
 * @brief GeneralizedHiddenMarkovModel specialization of StateTraits
 */
template<>
struct StateTraits<GeneralizedHiddenMarkovModel> {
  using State = tops::model::State<Model, IID>;

  using MatchState = SimpleState<Model, IID, 1, false>;
  using SilentState = SimpleState<Model, IID, 1, true>;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_GENERALIZED_HIDDEN_MARKOV_MODEL_STATE_
