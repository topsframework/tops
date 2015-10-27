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

#ifndef TOPS_MODEL_PROBABILISTIC_MODEL_STATE_CRTP_
#define TOPS_MODEL_PROBABILISTIC_MODEL_STATE_CRTP_

// Standard headers
#include <memory>

// ToPS headers
#include "model/ProbabilisticModelState.hpp"

// ToPS templates
#include "model/SimpleSerializer.tcc"

namespace tops {
namespace model {

// Forward declaration
template<typename Derived>
class ProbabilisticModelStateCrtp;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                      CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/**
 * @typedef ProbabilisticModelStateCrtpPtr
 * @brief Alias of pointer to ProbabilisticModelStateCrtp.
 */
template<typename Derived>
using ProbabilisticModelStateCrtpPtr
  = std::shared_ptr<ProbabilisticModelStateCrtp<Derived>>;

/**
 * @class ProbabilisticModelStateCrtp
 * @brief TODO
 */
template<typename Derived>
class ProbabilisticModelStateCrtp : public ProbabilisticModelState {
 public:
  // Alias
  using Base = void;
  using DerivedPtr = std::shared_ptr<Derived>;

  using SSPtr = SimpleSerializerPtr<Derived>;

  // Static methods
  template<typename... Args>
  static DerivedPtr make(Args&&... args);

  // Overriden methods
  SerializerPtr serializer(TranslatorPtr translator) override;

  // Virtual methods
  virtual void serialize(SSPtr serializer);

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
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

template<typename Derived>
template<typename... Args>
auto ProbabilisticModelStateCrtp<Derived>::make(Args&&... args)
    -> typename ProbabilisticModelStateCrtp<Derived>::DerivedPtr {
  return DerivedPtr(new Derived(std::forward<Args>(args)...));
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  SERIALIZER  ===============================*/

template<typename Derived>
SerializerPtr ProbabilisticModelStateCrtp<Derived>::serializer(
    TranslatorPtr translator) {
  return SimpleSerializer<Derived>::make(make_shared(), translator);
}

/*----------------------------------------------------------------------------*/
/*                              VIRTUAL METHODS                               */
/*----------------------------------------------------------------------------*/

/*===============================  SERIALIZER  ===============================*/

template<typename Derived>
void
ProbabilisticModelStateCrtp<Derived>::serialize(SSPtr serializer) {
  serializer->translator()->translate(this->make_shared());
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename Derived>
std::shared_ptr<Derived> ProbabilisticModelStateCrtp<Derived>::make_shared() {
  return std::static_pointer_cast<Derived>(
    static_cast<Derived *>(this)->shared_from_this());
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_PROBABILISTIC_STATE_MODEL_
