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

#ifndef STRING_LITERAL_SUFFIX_
#define STRING_LITERAL_SUFFIX_

// External headers
#include "named_types/named_tag.hpp"
#include "named_types/literals/string_literal.hpp"

namespace tops {
namespace config {

template<typename T, T... chars>
constexpr decltype(auto) operator ""_t () {
  return named_types::named_tag<named_types::string_literal<T, chars...>>{};
}

}  // namespace config
}  // namespace tops

#endif  // STRING_LITERAL_SUFFIX_
