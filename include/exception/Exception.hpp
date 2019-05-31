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

#ifndef TOPS_EXCEPTION_EXCEPTION_
#define TOPS_EXCEPTION_EXCEPTION_

// Standard headers
#include <string>
#include <exception>

// Macros
#define throw_exception(Exception) \
  throw tops::exception::Exception(__FILE__, __LINE__, __func__)

namespace tops {
namespace exception {

/**
 * @class Exception
 * @brief Exceptions base class. All ToPS exceptions should extends this class
 */
class Exception : public std::exception {
 public:
  // Constructors
  Exception(const char* file, size_t line, const char* func,
            std::string error_message);

  // Overriden methods
  const char* what() const noexcept final;

 protected:
  // Instance variables
  std::string _error_message;
};

}  // namespace exception
}  // namespace tops

#endif  // TOPS_EXCEPTION_EXCEPTION_
