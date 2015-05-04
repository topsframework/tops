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
#include "model/Generator.hpp"
#include "model/ProbabilisticModel.hpp"

namespace tops {
namespace model {

Generator::Generator(ProbabilisticModelPtr &&model)
    : _model(std::move(model)) {
}

inline Sequence Generator::choose(unsigned int size,
                                  unsigned int phase) const {
  Sequence sequence{};
  return _model->chooseSequence(sequence, size, phase);
}

}  // namespace model
}  // namespace tops
