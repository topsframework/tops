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

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_MATRIX_
