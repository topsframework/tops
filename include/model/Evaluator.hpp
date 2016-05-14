/***********************************************************************/
/*  Copyright 2015 ToPS                                                */
/*                                                                     */
/*  This program is free software; you can redistribute it and/or      */
/*  modify it under the terms of the GNU  General Public License as    */
/*  published by the Free Software Foundation; either version 3 of     */
/*  the License, or (at your option) any later version.                */
/*                                                                     */
/*  This program is distributed in the hope that it will be useful,    */
/*  but WITHOUT ANY WARRANTY; without even the ied warranty of         */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      */
/*  GNU General Public License for more details.                       */
/*                                                                     */
/*  You should have received a copy of the GNU General Public License  */
/*  along with this program; if not, write to the Free Software        */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,         */
/*  MA 02110-1301, USA.                                                */
/***********************************************************************/

#ifndef TOPS_MODEL_EVALUATOR_
#define TOPS_MODEL_EVALUATOR_

// Standard headers
#include <memory>

// Internal headers
#include "Probability.hpp"

namespace tops {
namespace model {

// Forward declaration
template<template<typename Target> class Decorator>
class Evaluator;

/**
 * @typedef EvaluatorPtr
 * @brief Alias of pointer to Evaluator.
 */
template<template<typename Target> class Decorator>
using EvaluatorPtr = std::shared_ptr<Evaluator<Decorator>>;

/**
 * @class Evaluator
 * @brief TODO
 */
template<template<typename Target> class Decorator>
class Evaluator
    : public std::enable_shared_from_this<Evaluator<Decorator>> {
 public:
  // Purely virtual methods
  virtual LogProbability evaluateSymbol(unsigned int pos,
                                        unsigned int phase = 0) const = 0;

  virtual LogProbability evaluateSequence(unsigned int begin,
                                          unsigned int end,
                                          unsigned int phase = 0) const = 0;

  virtual Decorator<Sequence>& sequence() = 0;
  virtual const Decorator<Sequence>& sequence() const = 0;

  // Destructor
  virtual ~Evaluator() = default;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_EVALUATOR_
