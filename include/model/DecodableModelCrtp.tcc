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

#ifndef TOPS_MODEL_DECODABLE_MODEL_CRTP_
#define TOPS_MODEL_DECODABLE_MODEL_CRTP_

// Standard headers
#include <memory>
#include <vector>

// ToPS headers
#include "model/Sequence.hpp"
#include "model/ProbabilisticModel.hpp"

// ToPS templates
#include "model/Labeling.tcc"
#include "model/Generator.tcc"
#include "model/SimpleGenerator.tcc"

namespace tops {
namespace model {

// Forward declaration
template<typename Derived>
class DecodableModelCrtp;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                      CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/**
 * @typedef DecodableModelCrtpPtr
 * @brief Alias of pointer to DecodableModelCrtp.
 */
template<typename Derived>
using DecodableModelCrtpPtr
  = std::shared_ptr<DecodableModelCrtp<Derived>>;

/**
 * @class DecodableModelCrtp
 * @brief TODO
 */
template<typename Derived>
class DecodableModelCrtp : public DecodableModel {
 public:
  // Alias
  using Base = DecodableModel;
  using DerivedPtr = std::shared_ptr<Derived>;

  template<typename Target>
  using SGPtr = SimpleGeneratorPtr<Target, Derived>;

  // Overriden methods
  GeneratorPtr<Labeling<Sequence>> labelingGenerator() override;

  // Virtual methods
  virtual Labeling<Sequence> simpleChoose(SGPtr<Labeling<Sequence>> generator,
                                          unsigned int size,
                                          unsigned int phase = 0) const = 0;
};

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                 IMPLEMENTATION
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename Derived>
GeneratorPtr<Labeling<Sequence>>
DecodableModelCrtp<Derived>::labelingGenerator() {

  return SimpleGenerator<Labeling<Sequence>, Derived>::make(
    std::static_pointer_cast<Derived>(
      static_cast<Derived *>(this)->shared_from_this()));
}

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_DECODABLE_MODEL_
