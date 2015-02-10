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
#include "model/ProbabilisticModelDecorator.hpp"
#include "model/DiscreteIIDModel.hpp"

namespace tops {
namespace model {

class FixedSequenceAtPosition;
using FixedSequenceAtPositionPtr = std::shared_ptr<FixedSequenceAtPosition>;

class FixedSequenceAtPosition : public ProbabilisticModelDecorator {
 public:
  // Static methods
  static FixedSequenceAtPositionPtr make(ProbabilisticModelPtr model, int position, Sequence sequence, DiscreteIIDModelPtr distr);

  // Virtual methods
  virtual double evaluateSequence(const Sequence &s,
                                  unsigned int begin,
                                  unsigned int end) const;
  virtual Sequence chooseSequence(Sequence &s, unsigned int size) const;
 private:
  int _position;
  Sequence _sequence;
  DiscreteIIDModelPtr _probabilities;

  void addSequence(Sequence & h) const;

  FixedSequenceAtPosition(ProbabilisticModelPtr model, int position, Sequence sequence, DiscreteIIDModelPtr distr);
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_FIXED_SEQUENCE_AT_POSITION_
