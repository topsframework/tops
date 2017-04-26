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
template<typename EmissionModel, typename TransitionModel,
         std::size_t dimensions, bool... gaps>
class SimpleState;

/**
 * @typedef SimpleStatePtr
 * @brief Alias of pointer to SimpleState.
 */
template<typename EmissionModel, typename TransitionModel,
         std::size_t dimensions, bool... gaps>
using SimpleStatePtr
  = std::shared_ptr<SimpleState<EmissionModel, TransitionModel,
                                dimensions, gaps...>>;

/**
 * @class SimpleState
 * @brief TODO
 */
template<typename EmissionModel, typename TransitionModel,
         std::size_t dimensions, bool... gaps>
class SimpleState
    : public StateCrtp<EmissionModel, TransitionModel,
                       SimpleState<EmissionModel, TransitionModel,
                                   dimensions, gaps...>> {
 public:
  // Assertions
  static_assert(dimensions >= 1,
      "Minimal dimension accepted is 1");
  static_assert(sizeof...(gaps) == dimensions,
      "Number of gaps is not the same of the indicated dimension");

  // Alias
  using Self = SimpleState<EmissionModel, TransitionModel,
                           dimensions, gaps...>;
  using SelfPtr = std::shared_ptr<Self>;

  using Base = StateCrtp<EmissionModel, TransitionModel, Self>;

  using Id = typename Base::Id;
  using Position = typename Base::Position;
  using Dimension = typename Base::Dimension;

  using EmissionModelPtr = typename Base::EmissionModelPtr;
  using TransitionModelPtr = typename Base::TransitionModelPtr;

  // Constructors
  SimpleState(Id id, EmissionModelPtr emission,
                     TransitionModelPtr transition);

  SimpleState(Id id, EmissionModelPtr emission,
                     TransitionModelPtr transition,
                     DurationPtr duration);

  // Overriden methods
  bool hasGap(Dimension dim) const override;
  bool isSilent() const override;

  Position delta(Dimension dim) const override;

 private:
  std::vector<bool> _gaps;
};

}  // namespace model
}  // namespace tops

// Implementation header
#include "model/SimpleState.ipp"

#endif  // TOPS_MODEL_SIMPLE_STATE_
