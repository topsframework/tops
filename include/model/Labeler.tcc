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

#ifndef TOPS_MODEL_LABELER_
#define TOPS_MODEL_LABELER_

// Standard headers
#include <memory>
#include <exception>

// ToPS headers
#include "Symbol.hpp"
#include "Sequence.hpp"

namespace tops {
namespace model {

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                    CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

template<template<typename Target> class Decorator>
class Labeler;

/**
 * @typedef LabelerPtr
 * @brief Alias of pointer to Labeler.
 */
template<template<typename Target> class Decorator>
using LabelerPtr = std::shared_ptr<Labeler<Decorator>>;

/**
 * @class Labeler
 * @brief TODO
 */
template<template<typename Target> class Decorator>
class Labeler : public std::enable_shared_from_this<Labeler<Decorator>> {
 public:
  // Purely virtual methods
  virtual Estimation<Labeling<Sequence>> labeling(
      Labeling<Sequence>::Method method) const = 0;

  virtual Decorator<Sequence>& sequence() = 0;
  virtual const Decorator<Sequence>& sequence() const = 0;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_LABELER_
