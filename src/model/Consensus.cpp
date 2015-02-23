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

// Standard headers
#include <cmath>
#include <vector>

// ToPS headers
#include "Consensus.hpp"

namespace tops {
namespace model {

Consensus::Consensus(const Sequence& symbols)
  : _symbols{symbols} {}

bool Consensus::is(unsigned int symbol) const {
  for (auto it = _symbols.begin() ; it != _symbols.end(); ++it) {
    if (*it == symbol)
      return true;
  }
  return false;
}

const Sequence Consensus::symbols() const {
  return _symbols;
}

}  // namespace model
}  // namespace tops
