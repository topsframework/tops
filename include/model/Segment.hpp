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

#ifndef TOPS_MODEL_SEGMENT_
#define TOPS_MODEL_SEGMENT_

// Standard headers
#include <vector>

// ToPS headers
#include "Symbol.hpp"
#include "Sequence.hpp"

namespace tops {
namespace model {

class Segment {
 public:
  // Constructors
  Segment(Symbol symbol, int begin, int end);

  // Static methods
  static std::vector<Segment> readSequence(const Sequence &s);

  // Concrete methods
  Symbol symbol();
  int begin();
  int end();

 private:
  // Instance variables
  Symbol _symbol;
  int _begin;
  int _end;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_SEGMENT_
