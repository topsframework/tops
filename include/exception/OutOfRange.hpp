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

#ifndef TOPS_EXCEPTION_OUT_OF_RANGE_
#define TOPS_EXCEPTION_OUT_OF_RANGE_

// Internal headers
#include "exception/Exception.hpp"

namespace tops {
namespace exception {

/**
 * @class OutOfRange
 * @brief Report an out of range exception
 */
class OutOfRange : public Exception {
 public:
  // Constructors
  OutOfRange(const char* file, size_t line, const char* func);
};

}  // namespace exception
}  // namespace tops

#endif  // TOPS_EXCEPTION_OUT_OF_RANGE_
