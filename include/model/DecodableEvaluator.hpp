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

#ifndef TOPS_MODEL_DECODABLE_EVALUATOR_
#define TOPS_MODEL_DECODABLE_EVALUATOR_

// Standard headers
#include <memory>

// ToPS headers
#include "model/Evaluator.hpp"
#include "model/EvaluatorImpl.hpp"

namespace tops {
namespace model {

class DecodableEvaluator;

/**
 * @typedef DecodableEvaluatorPtr
 * @brief Alias of pointer to DecodableEvaluator.
 */
using DecodableEvaluatorPtr = std::shared_ptr<DecodableEvaluator>;

/**
 * @class DecodableEvaluator
 * @brief TODO
 */
class DecodableEvaluator : public Evaluator {
 public:
  // Static methods
  template<typename... Ts>
  static DecodableEvaluatorPtr make(Ts... args) {
    return DecodableEvaluatorPtr(
        new DecodableEvaluator(std::forward<Ts>(args)...));
  }

 protected:
  // Constructors
  DecodableEvaluator(EvaluatorImplPtr &&impl);
};

}  // namespace model
}  // namespace tops

#endif
