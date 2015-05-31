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

#ifndef TOPS_MODEL_GENERATOR_
#define TOPS_MODEL_GENERATOR_

// Standard headers
#include <memory>
#include <exception>

// ToPS headers
#include "model/GeneratorImpl.hpp"

namespace tops {
namespace model {

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                    CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

template<typename Target>
class Generator;

/**
 * @typedef GeneratorPtr
 * @brief Alias of pointer to Generator.
 */
template<typename Target>
using GeneratorPtr = std::shared_ptr<Generator<Target>>;

/**
 * @class Generator
 * @brief TODO
 */
template<typename Target>
class Generator : public std::enable_shared_from_this<Generator<Target>> {
 public:
  // Static methods
  template<typename... Ts>
  static GeneratorPtr<Target> make(Ts... args);

  // Virtual methods
  virtual Target choose(unsigned int size,
                        unsigned int phase = 0) const;

 protected:
  // Instance variables
  GeneratorImplPtr _impl;

  // Constructors
  Generator(GeneratorImplPtr &&impl);
};

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                IMPLEMENTATION
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/*----------------------------------------------------------------------------*/
/*                               CONSTRUCTORS                                 */
/*----------------------------------------------------------------------------*/

template<typename Target>
Generator<Target>::Generator(GeneratorImplPtr &&impl)
    : _impl(std::move(impl)) {
}

/*----------------------------------------------------------------------------*/
/*                              STATIC METHODS                                */
/*----------------------------------------------------------------------------*/

template<typename Target>
template<typename... Ts>
GeneratorPtr<Target> Generator<Target>::make(Ts... args) {
  return GeneratorPtr<Target>(
    new Generator<Target>(std::forward<Ts>(args)...));
}

}  // namespace model
}  // namespace tops

#endif
