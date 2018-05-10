/***********************************************************************/
/*  Copyright 2016 ToPS                                                */
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

// Interface header
#include "lang/Util.hpp"

// Standard headers
#include <string>

namespace tops {
namespace lang {

/*----------------------------------------------------------------------------*/
/*                                 FUNCTIONS                                  */
/*----------------------------------------------------------------------------*/

std::string removeSubstring(const std::string &substr, std::string str) {
  if (substr.empty()) return str;

  auto it = str.find(substr);
  while (it != std::string::npos) {
    str.erase(it, substr.length());
    it = str.find(substr);
  }
  return str;
}

/*----------------------------------------------------------------------------*/

std::string cleanPath(std::string filepath) {
  return removeSubstring("//", removeSubstring("./", filepath));
}

/*----------------------------------------------------------------------------*/

std::string extractRoot(const std::string &filepath) {
  auto found = filepath.find_first_of("/\\");
  return cleanPath(filepath).substr(0, found + 1);
}

/*----------------------------------------------------------------------------*/

std::string extractCorename(const std::string &filepath) {
  auto found = filepath.find_first_of("/\\");
  return cleanPath(filepath).substr(found + 1);
}

/*----------------------------------------------------------------------------*/

std::string extractDir(const std::string &filepath) {
  auto found = filepath.find_last_of("/\\");
  return cleanPath(filepath).substr(0, found + 1);
}

/*----------------------------------------------------------------------------*/

std::string extractBasename(const std::string &filepath) {
  auto found = cleanPath(filepath).find_last_of("/\\");
  return filepath.substr(found + 1);
}

/*----------------------------------------------------------------------------*/

std::string extractSuffix(const std::string &filepath) {
  auto found = cleanPath(filepath).find_last_of(".");
  return filepath.substr(found + 1);
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
}  // namespace tops
