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

#ifndef TOPS_MODEL_GHMM_STATE_CRTP_
#define TOPS_MODEL_GHMM_STATE_CRTP_

// Standard headers
#include <memory>
#include <vector>

// ToPS headers
#include "model/Range.hpp"
#include "model/DiscreteIIDModel.hpp"
#include "model/ProbabilisticModel.hpp"
#include "model/GeneralizedHiddenMarkovModelState.hpp"

// ToPS templates
#include "model/ProbabilisticModelStateCrtp.tcc"

namespace tops {
namespace model {

// Forward declaration
template<typename Derived>
class GHMMStateCrtp;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                      CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/**
 * @typedef GHMMStateCrtpPtr
 * @brief Alias of pointer to GHMMStateCrtp.
 */
template<typename Derived>
using GHMMStateCrtpPtr
  = std::shared_ptr<GHMMStateCrtp<Derived>>;

/**
 * @class GHMMStateCrtp
 * @brief TODO
 */
template<typename Derived>
class GHMMStateCrtp
  : public ProbabilisticModelStateCrtp<Derived>,
    public virtual GeneralizedHiddenMarkovModelState {
 public:
  // Alias
  using Base = ProbabilisticModelStateCrtp<Derived>;
  using Id = typename ProbabilisticModelStateCrtp<Derived>::Id;

  using Self = GHMMStateCrtp<Derived>;
  using SelfPtr = std::shared_ptr<Self>;

  // Constructors
  GHMMStateCrtp(Id id,
                ProbabilisticModelPtr observation,
                DiscreteIIDModelPtr transition);

  // Overriden methods
  Id id() const override;

  ProbabilisticModelPtr observation() override;
  DiscreteIIDModelPtr transition() override;

  void addPredecessor(Id id) override;
  std::vector<Id>& predecessors() override;
  const std::vector<Id>& predecessors() const override;

  void addSuccessor(Id id) override;
  std::vector<Id>& successors() override;
  const std::vector<Id>& successors() const override;

 protected:
  // Instance variables
  Id _id;
  ProbabilisticModelPtr _observation;
  DiscreteIIDModelPtr _transition;
  std::vector<Id> _predecessors;
  std::vector<Id> _successors;
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

template<typename Derived>
GHMMStateCrtp<Derived>::GHMMStateCrtp(
    Id id,
    ProbabilisticModelPtr observation,
    DiscreteIIDModelPtr transition)
    : _id(std::move(id)),
      _observation(std::move(observation)),
      _transition(std::move(transition)) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename Derived>
auto GHMMStateCrtp<Derived>::id() const -> Id {
  return _id;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
ProbabilisticModelPtr GHMMStateCrtp<Derived>::observation() {
  return _observation;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
DiscreteIIDModelPtr GHMMStateCrtp<Derived>::transition() {
  return _transition;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
void GHMMStateCrtp<Derived>::addPredecessor(GHMMStateCrtp<Derived>::Id id) {
  _predecessors.push_back(id);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
auto GHMMStateCrtp<Derived>::predecessors() -> std::vector<Id>& {
  return _predecessors;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
auto GHMMStateCrtp<Derived>::predecessors() const -> const std::vector<Id>& {
  return _predecessors;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
void GHMMStateCrtp<Derived>::addSuccessor(GHMMStateCrtp<Derived>::Id id) {
  _successors.push_back(id);
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
auto GHMMStateCrtp<Derived>::successors() -> std::vector<Id>& {
  return _successors;
}

/*----------------------------------------------------------------------------*/

template<typename Derived>
auto GHMMStateCrtp<Derived>::successors() const -> const std::vector<Id>& {
  return _successors;
}

/*----------------------------------------------------------------------------*/


}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_HIDDEN_MARKOV_MODEL_STATE_CRTP_
