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

// ToPS headers
#include "CachedEvaluator.hpp"
#include "ProbabilisticModel.hpp"

namespace tops {
namespace model {

CachedEvaluator::CachedEvaluator(ProbabilisticModelPtr m, const Sequence &s, void *mem)
    : Evaluator(m, s), memory(mem) {
}

CachedEvaluatorPtr CachedEvaluator::make(ProbabilisticModelPtr m, const Sequence &s, void *memory) {
  auto evaluator = CachedEvaluatorPtr(new CachedEvaluator(m, s, memory));
  m->initializePrefixSumArray(evaluator);
  return evaluator;
}

double CachedEvaluator::probabilityOf(unsigned int begin,
                                      unsigned int end,
                                      unsigned int phase) {
  return _model->evaluateWithPrefixSumArray(std::static_pointer_cast<CachedEvaluator>(shared_from_this()), begin, end, phase);
}

}  // namespace model
}  // namespace tops