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

#ifndef EXCEPTION_NOT_YET_IMPLEMENTED_
#define EXCEPTION_NOT_YET_IMPLEMENTED_

// Standard headers
#include <string>
#include <sstream>

// ToPS headers
#include "exception/Exception.hpp"

namespace tops {
namespace exception {

/**
 * @class NotYetImplemented
 * @brief Report a not yet implemented exception
 */
class NotYetImplemented : public Exception {

public:
  NotYetImplemented(std::string method_name);
  const char* what() const throw() override;

private:
  std::string _method_name;
  std::string _error_message;

};

}  // namespace exception
}  // namespace tops

#endif  // EXCEPTION_NOT_YET_IMPLEMENTED_
