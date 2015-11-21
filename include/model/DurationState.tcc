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

#ifndef TOPS_MODEL_DURATION_STATE_
#define TOPS_MODEL_DURATION_STATE_

// Standard headers
#include <memory>
#include <vector>

// ToPS headers
#include "model/Duration.hpp"

// ToPS templates
#include "model/StateCrtp.tcc"

namespace tops {
namespace model {

// Forward declaration
template<typename EmissionModel, typename TransitionModel>
class DurationState;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                      CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/**
 * @typedef DurationStatePtr
 * @brief Alias of pointer to DurationState.
 */
template<typename EmissionModel, typename TransitionModel>
using DurationStatePtr
  = std::shared_ptr<DurationState<EmissionModel, TransitionModel>>;

/**
 * @class DurationState
 * @brief TODO
 */
template<typename EmissionModel, typename TransitionModel>
class DurationState
    : public StateCrtp<EmissionModel, TransitionModel,
                       DurationState<EmissionModel, TransitionModel>> {
 public:
  // Alias
  using Self = DurationState<EmissionModel, TransitionModel>;
  using SelfPtr = std::shared_ptr<Self>;

  using Base = StateCrtp<EmissionModel, TransitionModel, Self>;

  using Id = typename Base::Id;
  using EmissionModelPtr = typename Base::EmissionModelPtr;
  using TransitionModelPtr = typename Base::TransitionModelPtr;

  // Constructors
  DurationState(Id id, EmissionModelPtr emission,
                       TransitionModelPtr transition,
                       DurationPtr duration);

  // Virtual methods
  virtual DurationPtr duration();

  virtual RangePtr range() const;
  virtual unsigned int maximumDurationSize() const;
  virtual Probability durationProbability(unsigned int length) const;

 protected:
  // Instance variables
  DurationPtr _duration;
};

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                 IMPLEMENTATION
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

template<typename E, typename T>
DurationState<E, T>::DurationState(Id id, EmissionModelPtr emission,
                                          TransitionModelPtr transition,
                                          DurationPtr duration)
    : Base(std::move(id), std::move(emission), std::move(transition)),
      _duration(std::move(duration)) {
}

/*----------------------------------------------------------------------------*/
/*                              VIRTUAL METHODS                               */
/*----------------------------------------------------------------------------*/

template<typename E, typename T>
DurationPtr DurationState<E, T>::duration() {
  return _duration;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T>
RangePtr DurationState<E, T>::range() const {
  return _duration->range();
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T>
unsigned int DurationState<E, T>::maximumDurationSize() const {
  return _duration->maximumDurationSize();
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T>
Probability
DurationState<E, T>::durationProbability(unsigned int length) const {
  return _duration->durationProbability(length);
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_STATE_
