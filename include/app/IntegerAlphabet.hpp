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

#ifndef TOPS_APP_INTEGER_ALPHABET_
#define TOPS_APP_INTEGER_ALPHABET_

// Internal headers
#include "app/Alphabet.hpp"

#include "model/Symbol.hpp"

namespace tops {
namespace app {

// Forward declaration
class IntegerAlphabet;

using IntegerAlphabetPtr
  = std::shared_ptr<IntegerAlphabet>;

class IntegerAlphabet : public Alphabet {
  model::Symbol getID(std::string symbol) override {
    return std::stoi(symbol);
  }
};

}  // namespace app
}  // namespace tops

#endif  // TOPS_APP_INTEGER_ALPHABET_
