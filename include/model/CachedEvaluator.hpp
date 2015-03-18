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

#ifndef TOPS_MODEL_CACHED_EVALUATOR_MODEL_
#define TOPS_MODEL_CACHED_EVALUATOR_MODEL_

// Standard headers
#include <memory>

// ToPS headers
#include "model/Evaluator.hpp"
#include "model/Sequence.hpp"

namespace tops {
namespace model {

class CachedEvaluator;

/**
 * @typedef CachedEvaluatorPtr
 * @brief Alias of pointer to CachedEvaluator.
 */
using CachedEvaluatorPtr = std::shared_ptr<CachedEvaluator>;

/**
 * @class CachedEvaluator
 * @brief TODO
 */
class CachedEvaluator : public Evaluator {
 public:
  // Static methods
  static CachedEvaluatorPtr make(ProbabilisticModelPtr m, const Sequence &s);

  // Concrete methods
  virtual double probabilityOf(unsigned int begin,
                               unsigned int end,
                               unsigned int phase = 0) const;
 private:
  // Constructors
  CachedEvaluator(ProbabilisticModelPtr m, const Sequence &s);
};

}  // namespace model
}  // namespace tops

#endif
