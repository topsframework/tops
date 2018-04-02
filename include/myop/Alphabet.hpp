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

#ifndef TOPS_APP_ALPHABET_
#define TOPS_APP_ALPHABET_

// Standard headers
#include <map>
#include <memory>
#include <vector>

// Internal headers
#include "model/Symbol.hpp"

namespace tops {
namespace myop {

// Forward declaration
class Alphabet;

using AlphabetPtr
  = std::shared_ptr<Alphabet>;

class Alphabet {
 public:
  void addSymbol(std::string symbol) {
    _hash[symbol] = _vector.size();
    _vector.push_back(symbol);
  }

  virtual model::Symbol getID(std::string symbol) {
    return _hash[symbol];
  }

  std::size_t size() {
    return _vector.size();
  }

  virtual ~Alphabet() = default;

 private:
  std::map<std::string, model::Symbol> _hash;
  std::vector<std::string> _vector;
};

}  // namespace myop
}  // namespace tops

#endif  // TOPS_APP_ALPHABET_
