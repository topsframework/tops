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

#ifndef TOPS_MODEL_PROBABILISTIC_MODEL_
#define TOPS_MODEL_PROBABILISTIC_MODEL_

// Standard headers
#include <memory>

// ToPS headers
#include "model/Sequence.hpp"

namespace tops {
namespace model {

class ProbabilisticModel {
 public:
  // Purely virtual methods
  virtual int alphabetSize() const = 0;
  virtual double evaluateSequence(const Sequence &s,
                                  unsigned int begin,
                                  unsigned int end) const = 0;
  virtual double evaluatePosition(const Sequence &s, unsigned int i) const = 0;
  virtual Symbol choosePosition(const Sequence &s, unsigned int i) const = 0;
  virtual Sequence chooseSequence(Sequence &s, unsigned int size) const = 0;
};

typedef std::shared_ptr<ProbabilisticModel> ProbabilisticModelPtr;

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_PROBABILISTIC_MODEL_
