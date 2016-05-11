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

#ifndef TOPS_MODEL_SIMPLE_STATE_
#define TOPS_MODEL_SIMPLE_STATE_

// Standard headers
#include <memory>
#include <vector>

// Internal headers
#include "model/StateCrtp.hpp"

namespace tops {
namespace model {

// Forward declaration
template<typename EmissionModel, typename TransitionModel>
class SimpleState;

/**
 * @typedef SimpleStatePtr
 * @brief Alias of pointer to SimpleState.
 */
template<typename EmissionModel, typename TransitionModel>
using SimpleStatePtr
  = std::shared_ptr<SimpleState<EmissionModel, TransitionModel>>;

/**
 * @class SimpleState
 * @brief TODO
 */
template<typename EmissionModel, typename TransitionModel>
class SimpleState
    : public StateCrtp<EmissionModel, TransitionModel,
                       SimpleState<EmissionModel, TransitionModel>> {
 public:
  // Alias
  using Self = SimpleState<EmissionModel, TransitionModel>;
  using SelfPtr = std::shared_ptr<Self>;

  using Base = StateCrtp<EmissionModel, TransitionModel, Self>;

  using Id = typename Base::Id;
  using EmissionModelPtr = typename Base::EmissionModelPtr;
  using TransitionModelPtr = typename Base::TransitionModelPtr;

  // Constructors
  SimpleState(Id id, EmissionModelPtr emission,
                     TransitionModelPtr transition);
};

}  // namespace model
}  // namespace tops

// Implementation header
#include "model/SimpleState.ipp"

#endif  // TOPS_MODEL_SIMPLE_STATE_
