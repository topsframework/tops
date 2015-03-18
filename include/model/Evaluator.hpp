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

#ifndef TOPS_MODEL_EVALUATOR_MODEL_
#define TOPS_MODEL_EVALUATOR_MODEL_

// Standard headers
#include <memory>

// ToPS headers
#include "model/Sequence.hpp"

namespace tops {
namespace model {

class ProbabilisticModel;
using ProbabilisticModelPtr = std::shared_ptr<ProbabilisticModel>;

class Evaluator;

/**
 * @typedef DiscreteIIDModelPtr
 * @brief Alias of pointer to DiscreteIIDModel.
 */
using EvaluatorPtr = std::shared_ptr<Evaluator>;

/**
 * @class Evaluator
 * @brief TODO
 */
class Evaluator {
 public:
  // Static methods
  static EvaluatorPtr make(ProbabilisticModelPtr m, const Sequence &s);

  // Concrete methods
  double probabilityOf(unsigned int begin,
                       unsigned int end,
                       unsigned int phase = 0) const;
 private:
  // Instace variables
  ProbabilisticModelPtr _model;
  Sequence _sequence;

  // Constructors
  Evaluator(ProbabilisticModelPtr m, const Sequence &s);
};

}  // namespace model
}  // namespace tops

#endif
