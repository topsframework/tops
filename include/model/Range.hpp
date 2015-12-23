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

#ifndef TOPS_MODEL_RANGE_
#define TOPS_MODEL_RANGE_

// Standard headers
#include <memory>

namespace tops {
namespace model {

class Range;

/**
 * @typedef RangePtr
 * @brief Alias of pointer to Range.
 */
using RangePtr = std::shared_ptr<Range>;

/**
 * @class Range
 * @brief TODO
 */
class Range {
 public:
  // Purely virtual methods
  virtual unsigned int begin() = 0;
  virtual unsigned int next() = 0;
  virtual bool end() = 0;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_RANGE_
