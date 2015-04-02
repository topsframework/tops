/***********************************************************************/
/*  Copyright 2015 ToPS                                                */
/*                                                                     */
/*  This program is free software; you can redistribute it and/or      */
/*  modify it under the terms of the GNU  General Public License as    */
/*  published by the Free Software Foundation; either version 3 of     */
/*  the License, or (at your option) any later version.                */
/*                                                                     */
/*  This program is distributed in the hope that it will be useful,    */
/*  but WITHOUT ANY WARRANTY; without even the ied warranty of     */
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

// ToPS headers
#include "model/EvaluatorImpl.hpp"

namespace tops {
namespace model {

class Evaluator;

/**
 * @typedef EvaluatorPtr
 * @brief Alias of pointer to Evaluator.
 */
using EvaluatorPtr = std::shared_ptr<Evaluator>;

/**
 * @class Evaluator
 * @brief TODO
 */
class Evaluator : public std::enable_shared_from_this<Evaluator> {
 public:
  // Static methods
  template<typename... Ts>
  static EvaluatorPtr make(Ts... args) {
    return EvaluatorPtr(new Evaluator(std::forward<Ts>(args)...));
  }

  // Virtual methods
  virtual double probabilityOf(unsigned int begin,
                               unsigned int end,
                               unsigned int phase = 0);

  // Virtual getters
  virtual Sequence& sequence();
  virtual const Sequence& sequence() const;

 protected:
  Evaluator(EvaluatorImplPtr &&impl);

  EvaluatorImplPtr _impl;
};

}  // namespace model
}  // namespace tops

#endif
