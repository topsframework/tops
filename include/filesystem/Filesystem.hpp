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

#ifndef FILESYSTEM_FILESYSTEM_
#define FILESYSTEM_FILESYSTEM_

// Standard headers
#include <string>

namespace filesystem {

bool is_directory(const std::string& path, int &error_code) noexcept;
bool is_directory(const std::string& path);

bool create_directory(const std::string& path, int &error_code) noexcept;
bool create_directory(const std::string& path);

bool create_directories(const std::string& path, int &error_code) noexcept;
bool create_directories(const std::string& path);

}  // namespace filesystem

#endif  // FILESYSTEM_FILESYSTEM_
