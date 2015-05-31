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

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                             VIRTUAL METHODS                                */
/*----------------------------------------------------------------------------*/

template<typename Target>
Target Generator<Target>::choose(unsigned int size,
                                 unsigned int phase) const {
  throw std::logic_error("Unavaiable implementation for Target");
}

template<>
Sequence Generator<Sequence>::choose(unsigned int size,
                                     unsigned int phase) const {
  return _impl->chooseSequence(size, phase);
}

template<>
Labeling<Sequence>
Generator<Labeling<Sequence>>::choose(unsigned int size,
                                      unsigned int phase) const {
  return _impl->chooseLabeling(size, phase);
}

}  // namespace model
}  // namespace tops
