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

#ifndef TOPS_HELPER_SEQUENCE_
#define TOPS_HELPER_SEQUENCE_

// Standard headers
#include <vector>

// ToPS headers
#include "model/Sequence.hpp"

namespace tops {
namespace helper {

model::Sequence generateRandomSequence(unsigned int size,
                                       unsigned int alphabet_size);

model::Sequence sequenceOfLengths();

std::vector<model::Sequence>
generateAllCombinationsOfSymbols(unsigned int size);

}  // namespace helper
}  // namespace tops

#endif  // TOPS_HELPER_SEQUENCE_
