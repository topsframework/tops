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

#ifndef TOPS_MODEL_State_
#define TOPS_MODEL_State_

// Standard headers
#include <memory>
#include <vector>

// ToPS templates
#include "model/Serializer.tcc"

namespace tops {
namespace model {

// Forward declaration
template<typename EmissionModel, typename TransitionModel>
class State;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                      CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/**
 * @typedef StatePtr
 * @brief Alias of pointer to State.
 */
template<typename EmissionModel, typename TransitionModel>
using StatePtr
  = std::shared_ptr<State<EmissionModel, TransitionModel>>;

/**
 * @class State
 * @brief TODO
 */
template<typename EmissionModel, typename TransitionModel>
class State {
 public:
  // Alias
  using Self = State<EmissionModel, TransitionModel>;
  using SelfPtr = std::shared_ptr<Self>;

  using Id = unsigned int;
  using EmissionModelPtr = std::shared_ptr<EmissionModel>;
  using TransitionModelPtr = std::shared_ptr<TransitionModel>;

  // Purely virtual methods
  virtual SerializerPtr serializer(TranslatorPtr translator) = 0;

  virtual Id id() const = 0;
  virtual EmissionModelPtr emission() = 0;
  virtual TransitionModelPtr transition() = 0;

  virtual void addPredecessor(Id id) = 0;
  virtual std::vector<Id>& predecessors() = 0;
  virtual const std::vector<Id>& predecessors() const = 0;

  virtual void addSuccessor(Id id) = 0;
  virtual std::vector<Id>& successors() = 0;
  virtual const std::vector<Id>& successors() const = 0;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_State_
