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

#ifndef TOPS_MODEL_MATRIX_
#define TOPS_MODEL_MATRIX_

// Standard headers
#include <array>
#include <vector>

// External headers
#include "multivector/multivector.hpp"

// Internal headers
#include "model/Probability.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/

template<typename T>
using View = multivector::View<T>;
template<typename T>
using MultiVector = multivector::MultiVector<T>;

using Matrix = MultiVector<Probability>;
using Cube = MultiVector<Probability>;

/*----------------------------------------------------------------------------*/

template<typename Original>
MultiVector<Probability> normalize(const View<MultiVector<Original>>& values) {
  MultiVector<Probability> converted(values.size());

  Original sum;
  for (const auto& value : values) {
    sum += values;
  }

  for (size_t i = 0; i < values.size(); i++) {
    converted[i] = values[i] / sum;
  }

  return converted;
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_MATRIX_
