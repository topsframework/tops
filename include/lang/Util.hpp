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

#ifndef LANG_UTIL_
#define LANG_UTIL_

// Standard headers
#include <string>
#include <utility>
#include <iterator>
#include <type_traits>

namespace tops {
namespace lang {
namespace detail {

// To allow ADL with custom begin/end and custom to_string
using std::begin;
using std::end;
using std::to_string;

// is_iterable

template<typename T>
auto is_iterable_impl(void * = nullptr)
    -> decltype(begin(std::declval<T>()) != end(std::declval<T>()),
                ++std::declval<decltype(begin(std::declval<T>()))>(),
                *begin(std::declval<T>()),
                std::true_type{});

template<typename T>
std::false_type is_iterable_impl(...);

// is_pair

template<typename>
struct is_pair_impl : std::false_type {};

template<typename F, typename S>
struct is_pair_impl<std::pair<F, S>> : std::true_type {};

}  // namespace detail
}  // namespace lang
}  // namespace tops

namespace tops {
namespace lang {

template <typename T>
using is_iterable = decltype(detail::is_iterable_impl<T>(nullptr));

template <typename T>
using is_pair = detail::is_pair_impl<T>;

// Path manipulation

std::string removeSubstring(const std::string &substr, std::string str);

std::string cleanPath(std::string filepath);

std::string extractRoot(const std::string &filepath);
std::string extractCorename(const std::string &filepath);
std::string extractDir(const std::string &filepath);
std::string extractBasename(const std::string &filepath);
std::string extractSuffix(const std::string &filepath);

}  // namespace lang
}  // namespace tops

#endif  // LANG_UTIL_
