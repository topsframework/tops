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

#ifndef TOPS_MODEL_FIXED_SEQUENCE_AT_POSITION_
#define TOPS_MODEL_FIXED_SEQUENCE_AT_POSITION_

// Standard headers
#include <memory>
#include <vector>

// ToPS headers
#include "model/DiscreteIIDModel.hpp"

// ToPS templates
#include "model/ProbabilisticModelDecoratorCrtp.tcc"

namespace tops {
namespace model {

class FixedSequenceAtPosition;

/**
 * @typedef FixedSequenceAtPositionPtr
 * @brief Alias of pointer to FixedSequenceAtPosition.
 */
using FixedSequenceAtPositionPtr = std::shared_ptr<FixedSequenceAtPosition>;

/**
 * @class FixedSequenceAtPosition
 * @brief TODO
 */
class FixedSequenceAtPosition
    : public ProbabilisticModelDecoratorCrtp<FixedSequenceAtPosition> {
 public:
  // Alias
  using Base = ProbabilisticModelDecoratorCrtp<FixedSequenceAtPosition>;

  // Constructors
  FixedSequenceAtPosition(ProbabilisticModelPtr model,
                          int position,
                          Sequence sequence,
                          DiscreteIIDModelPtr distr);

  // Overriden methods
  Probability evaluateSequence(SEPtr<Standard> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  Standard<Sequence> drawSequence(SGPtr<Standard> generator,
                                  unsigned int size,
                                  unsigned int phase = 0) const override;

 private:
  // Instance variables
  int _position;
  Sequence _sequence;
  DiscreteIIDModelPtr _probabilities;

  // Concrete methods
  void addSequence(Sequence &h) const;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_FIXED_SEQUENCE_AT_POSITION_
