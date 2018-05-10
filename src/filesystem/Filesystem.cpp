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
#include "filesystem/Filesystem.hpp"

// Standard headers
#include <string>
#include <system_error>

// Platform-dependent C headers
#if defined(_WIN32)
#include <direct.h>  // _mkdir
#endif

// POSIX headers
#include <errno.h>     // errno, ENOENT, EEXIST
#include <sys/stat.h>  // stat

// Macros
#if defined(_WIN32)

#define SEPARATOR '\\'
#define STAT(...) _stat(__VA_ARGS__)
#define MKDIR(STRING) _mkdir((STRING))

#else  // Linux and Apple

#define SEPARATOR '/'
#define STAT(...) stat(__VA_ARGS__)
#define MKDIR(STRING) mkdir((STRING), 0755)

#endif

namespace filesystem {

static std::string get_parent_path(const std::string &path) {
  int pos = path.find_last_of(SEPARATOR);
  return path.substr(0, pos);
}

bool is_directory(const std::string &path, int &error_code) noexcept {
  struct stat info;
  error_code = STAT(path.c_str(), &info);
  return (info.st_mode & S_IFDIR) != 0;
}

bool is_directory(const std::string &path) {
  int error_code = 0;
  if (is_directory(path, error_code)) return true;
  throw std::system_error(error_code, std::system_category());
}

bool create_directory(const std::string &path, int &error_code) noexcept {
  if (MKDIR(path.c_str()) == 0) return true;
  error_code = errno;

  switch (error_code) {
    case EEXIST: {  // already exists: check if is a directory
      int inner_error_code = 0;
      return is_directory(path, inner_error_code);
    }

    default:
      return false;
  }
}

bool create_directory(const std::string &path) {
  int error_code = 0;
  if (create_directory(path, error_code)) return true;
  throw std::system_error(error_code, std::system_category());
}

bool create_directories(const std::string &path, int &error_code) noexcept {
  if (create_directory(path, error_code)) return true;

  switch (error_code) {
    case ENOENT:  // parent didn't exist: try to create it
      if (!create_directories(get_parent_path(path), error_code)) return false;
      return create_directory(path, error_code);

    case EEXIST:  // already exists: check if is a directory
      return is_directory(path, error_code);

    default:
      return false;
  }
}

bool create_directories(const std::string &path) {
  int error_code = 0;
  if (create_directories(path, error_code)) return true;
  throw std::system_error(error_code, std::system_category());
}

}  // namespace filesystem
