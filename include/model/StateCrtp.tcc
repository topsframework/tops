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

#ifndef TOPS_MODEL_STATE_CRTP_
#define TOPS_MODEL_STATE_CRTP_

// Standard headers
#include <memory>
#include <vector>

// ToPS templates
#include "model/State.tcc"
#include "model/SimpleSerializer.tcc"

namespace tops {
namespace model {

// Forward declaration
template<typename EmissionModel, typename TransitionModel, typename Derived>
class StateCrtp;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                      CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/**
 * @typedef StateCrtpPtr
 * @brief Alias of pointer to StateCrtp.
 */
template<typename EmissionModel, typename TransitionModel, typename Derived>
using StateCrtpPtr
  = std::shared_ptr<StateCrtp<EmissionModel, TransitionModel, Derived>>;

/**
 * @class StateCrtp
 * @brief TODO
 */
template<typename EmissionModel, typename TransitionModel, typename Derived>
class StateCrtp
    : public std::enable_shared_from_this<
               StateCrtp<EmissionModel, TransitionModel, Derived>>,
      public virtual State<EmissionModel, TransitionModel> {
 public:
  // Alias
  using Self = StateCrtp<EmissionModel, TransitionModel, Derived>;
  using SelfPtr = std::shared_ptr<Self>;
  using Base = void;

  using DerivedPtr = std::shared_ptr<Derived>;

  using Id = unsigned int;
  using EmissionModelPtr = std::shared_ptr<EmissionModel>;
  using TransitionModelPtr = std::shared_ptr<TransitionModel>;

  using SSPtr = SimpleSerializerPtr<Derived>;

  // Static methods
  template<typename... Args>
  static DerivedPtr make(Args&&... args);

  // Virtual methods
  SerializerPtr serializer(TranslatorPtr translator) override;

  Id id() const override;
  EmissionModelPtr emission() override;
  TransitionModelPtr transition() override;

  void addPredecessor(Id id) override;
  std::vector<Id>& predecessors() override;
  const std::vector<Id>& predecessors() const override;

  void addSuccessor(Id id) override;
  std::vector<Id>& successors() override;
  const std::vector<Id>& successors() const override;

  // Virtual methods
  virtual void serialize(SSPtr serializer);

 protected:
  // Constructors
  StateCrtp(Id id, EmissionModelPtr emission,
                   TransitionModelPtr transition);

  // Instance variables
  Id _id;
  EmissionModelPtr _emission;
  TransitionModelPtr _transition;
  std::vector<Id> _predecessors;
  std::vector<Id> _successors;

 private:
  // Concrete methods
  DerivedPtr make_shared();
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

template<typename E, typename T, typename D>
StateCrtp<E, T, D>::StateCrtp(Id id, EmissionModelPtr emission,
                                     TransitionModelPtr transition)
    : _id(std::move(id)),
      _emission(std::move(emission)),
      _transition(std::move(transition)) {
}

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
template<typename... Args>
auto StateCrtp<E, T, D>::make(Args&&... args) -> DerivedPtr {
  return std::make_shared<D>(std::forward<Args>(args)...);
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  SERIALIZER  ===============================*/

template<typename E, typename T, typename D>
SerializerPtr StateCrtp<E, T, D>::serializer(TranslatorPtr translator) {
  return SimpleSerializer<D>::make(make_shared(), translator);
}

/*=================================  OTHERS  =================================*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::id() const -> Id {
  return _id;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::emission() -> EmissionModelPtr {
  return _emission;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::transition() -> TransitionModelPtr {
  return _transition;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
void StateCrtp<E, T, D>::addPredecessor(Id id) {
  _predecessors.push_back(id);
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::predecessors() -> std::vector<Id>& {
  return _predecessors;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::predecessors() const -> const std::vector<Id>& {
  return _predecessors;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
void StateCrtp<E, T, D>::addSuccessor(Id id) {
  _successors.push_back(id);
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::successors() -> std::vector<Id>& {
  return _successors;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::successors() const -> const std::vector<Id>& {
  return _successors;
}

/*----------------------------------------------------------------------------*/
/*                              VIRTUAL METHODS                               */
/*----------------------------------------------------------------------------*/

/*===============================  SERIALIZER  ===============================*/

template<typename E, typename T, typename D>
void StateCrtp<E, T, D>::serialize(SSPtr serializer) {
  serializer->translator()->translate(this->make_shared());
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::make_shared() -> DerivedPtr {
  return std::static_pointer_cast<D>(
    static_cast<D *>(this)->shared_from_this());
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_STATE_CRTP_
