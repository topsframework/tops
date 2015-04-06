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

// Libraries
#include "Labeling.hpp"

namespace tops {
namespace model {

Labeling::Labeling()
    : _probability(), _sequence() {
}

Labeling::Labeling(double probability, const Sequence& sequence)
    : _probability(probability), _sequence(sequence) {
}

Labeling::Labeling(double probability, Sequence&& sequence)
    : _probability(probability), _sequence(std::move(sequence)) {
}

double Labeling::probability() const {
  return _probability;
}

Sequence& Labeling::sequence() {
  return _sequence;
}

const Sequence& Labeling::sequence() const {
  return _sequence;
}

}  // namespace model
}  // namespace tops
