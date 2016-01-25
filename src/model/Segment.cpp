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
#include <vector>

// Interface header
#include "model/Segment.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

Segment::Segment(Symbol symbol, int begin, int end)
    :_symbol(symbol), _begin(begin), _end(end) {
}

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

std::vector<Segment> Segment::readSequence(const Sequence &s) {
  std::vector<Segment> segments;
  Symbol symbol = s[0];
  int begin = 0;
  for (unsigned int i = 1; i < s.size(); i++) {
    if (s[i] != symbol) {
      segments.push_back(Segment(symbol, begin, i));
      symbol = s[i];
      begin = i;
    }
  }
  segments.push_back(Segment(symbol, begin, s.size()));
  return segments;
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

Symbol Segment::symbol() {
  return _symbol;
}

/*----------------------------------------------------------------------------*/

int Segment::begin() {
  return _begin;
}

/*----------------------------------------------------------------------------*/

int Segment::end() {
  return _end;
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
