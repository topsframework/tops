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

#ifndef TOPS_MODEL_DURATION_CRTP_
#define TOPS_MODEL_DURATION_CRTP_

// Standard headers
#include <memory>

// Internal headers
#include "model/Duration.hpp"
#include "model/SimpleSerializer.hpp"

namespace tops {
namespace model {

// Forward declaration
template<typename Derived>
class DurationCrtp;

/**
 * @typedef DurationCrtpPtr
 * @brief Alias of pointer to DurationCrtp.
 */
template<typename Derived>
using DurationCrtpPtr = std::shared_ptr<DurationCrtp<Derived>>;

/**
 * @class DurationCrtp
 * @brief TODO
 */
template<typename Derived>
class DurationCrtp
    : public std::enable_shared_from_this<DurationCrtp<Derived>>,
      public virtual Duration {
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

}  // namespace model
}  // namespace tops

// Implementation header
#include "model/DurationCrtp.ipp"

#endif  // TOPS_MODEL_DURATION_CRTP_
