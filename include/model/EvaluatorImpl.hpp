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

#ifndef TOPS_MODEL_EVALUATOR_IMPL_
#define TOPS_MODEL_EVALUATOR_IMPL_

// ToPS headers
#include "model/Sequence.hpp"

// ToPS templates
#include "model/Labeling.tcc"
#include "model/Estimation.tcc"

namespace tops {
namespace model {

class EvaluatorImpl;

/**
 * @typedef EvaluatorImplPtr
 * @brief Alias of pointer to EvaluatorImpl.
 */
using EvaluatorImplPtr = std::shared_ptr<EvaluatorImpl>;

/**
 * @class EvaluatorImpl
 * @brief TODO
 */
class EvaluatorImpl : public std::enable_shared_from_this<EvaluatorImpl> {
 public:
  // Virtual methods
  virtual double probabilityOf(unsigned int begin,
                               unsigned int end,
                               unsigned int phase = 0) = 0;
  virtual double probabilityOf(const Sequence &s,
                               unsigned int begin,
                               unsigned int end) = 0;

  virtual Estimation<Labeling<Sequence>>
  labeling(Labeling<Sequence>::Method method) = 0;

  // Virtual getters
  virtual Sequence& sequence() = 0;
  virtual const Sequence& sequence() const = 0;
};

}  // namespace model
}  // namespace tops

#endif
