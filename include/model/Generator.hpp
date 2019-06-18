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

// Internal headers
#include "Symbol.hpp"
#include "Sequence.hpp"
#include "RandomNumberGenerator.hpp"

namespace tops {
namespace model {

// Forward declaration
template<template<typename Target> class Decorator>
class Generator;

/**
 * @typedef GeneratorPtr
 * @brief Alias of pointer to Generator.
 */
template<template<typename Target> class Decorator>
using GeneratorPtr = std::shared_ptr<Generator<Decorator>>;

/**
 * @class Generator
 * @brief TODO
 */
template<template<typename Target> class Decorator>
class Generator : public std::enable_shared_from_this<Generator<Decorator>> {
 public:
  // Purely virtual methods
  virtual Decorator<Symbol> drawSymbol(
      size_t pos,
      size_t phase = 0,
      const Decorator<Sequence>& context = {}) const = 0;

  virtual Decorator<Sequence> drawSequence(size_t size,
                                           size_t phase = 0) const = 0;

  virtual RandomNumberGeneratorPtr randomNumberGenerator() const = 0;

  // Destructor
  virtual ~Generator() = default;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_GENERATOR_
