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
#include "DecodableEvaluator.hpp"

namespace tops {
namespace model {

DecodableEvaluator::DecodableEvaluator(EvaluatorImplPtr &&impl)
    : Evaluator(std::move(impl)) {
}

inline double DecodableEvaluator::probabilityOf(const Sequence &s,
                                                unsigned int begin,
                                                unsigned int end) {
  return _impl->probabilityOf(s, begin, end);
}

inline Estimation<Labeling<Sequence>>
DecodableEvaluator::labeling(Labeling<Sequence>::Method method) const {
  return _impl->labeling(method);
}

}  // namespace model
}  // namespace tops
