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
#include <vector>

// Internal headers
#include "model/Probability.hpp"

namespace tops {
namespace model {

template<typename T, std::size_t N>
struct Nest : Nest<std::vector<T>, N-1> {};
template<typename T>
struct Nest<T, 0> { using type = T; };

template<typename T, std::size_t N>
using MultiArray = typename Nest<T, N>::type;

using Matrix = MultiArray<Probability, 2>;
using Cube = MultiArray<Probability, 3>;

/*----------------------------------------------------------------------------*/

template<typename Original, std::size_t N>
struct SumAux {
  Original operator()(const MultiArray<Original, N>& values) {
    Original acc {};
    for (const auto& curr : values) {
      acc += SumAux<Original, N-1>{}(curr);
    }
    return acc;
  }
};

template<typename Original>
struct SumAux<Original, 0> {
  Original operator()(const MultiArray<Original, 0>& values) {
    return values;
  }
};

template<typename Original, std::size_t N>
struct NormalizeAux {
  MultiArray<Probability, N>
  operator()(const MultiArray<Original, N>& values, Original sum) {
    MultiArray<Probability, N> converted;

    for (const auto& curr : values) {
      converted.push_back(NormalizeAux<Original, N-1>{}(curr, sum));
    }

    return converted;
  }
};

template<typename Original>
struct NormalizeAux<Original, 0> {
  MultiArray<Probability, 0>
  operator()(const MultiArray<Original, 0>& values, Original sum) {
    return Probability{ (1.0 * values) / sum };
  }
};

template<typename Original, std::size_t N>
MultiArray<Probability, N> normalize(const MultiArray<Original, N>& values) {
  return NormalizeAux<Original, N>{}(values, SumAux<Original, N>{}(values));
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_MATRIX_
