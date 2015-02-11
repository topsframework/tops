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

#ifndef TOPS_MODEL_PROBABILISTIC_MODEL_DECORATOR_
#define TOPS_MODEL_PROBABILISTIC_MODEL_DECORATOR_

// Standard headers
#include <memory>

// ToPS headers
#include "model/ProbabilisticModel.hpp"

namespace tops {
namespace model {

class ProbabilisticModelDecorator;

/**
 * @typedef ProbabilisticModelDecoratorPtr
 * @brief Alias of pointer to ProbabilisticModelDecorator.
 */
using ProbabilisticModelDecoratorPtr
    = std::shared_ptr<ProbabilisticModelDecorator>;

/**
 * @class ProbabilisticModelDecorator
 * @brief Base class that defines probabilistic models' decorators.
 *
 * It is the easiest way to change the behaviour of a probabilistic model.
 */
class ProbabilisticModelDecorator : public ProbabilisticModel {
 public:
  // Static methods
  static ProbabilisticModelDecoratorPtr make(ProbabilisticModelPtr model);
  // Virtual methods
  virtual int alphabetSize() const;
  virtual double evaluateSequence(const Sequence &s,
                                  unsigned int begin,
                                  unsigned int end) const;
  virtual double evaluatePosition(const Sequence &s, unsigned int i) const;
  virtual Symbol choosePosition(const Sequence &s, unsigned int i) const;
  virtual Sequence chooseSequence(Sequence &s, unsigned int size) const;
 protected:
  // Instance variables
  ProbabilisticModelPtr _model;

  // Constructors
  explicit ProbabilisticModelDecorator(ProbabilisticModelPtr model);
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_PROBABILISTIC_MODEL_DECORATOR_
