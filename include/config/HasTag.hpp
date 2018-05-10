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

#ifndef CONFIG_HAS_TAG_
#define CONFIG_HAS_TAG_

// External headers
#include "named_types/extensions/type_traits.hpp"

namespace tops {
namespace config {

/**
 * @fn has_tag
 * @brief Constexpr function to check if named_tuple has a tag
 * @tparam Tag String literal to be looked up in list of types
 * @tparam Types List of Type(decltype("name"_t)) to search for tag
 */
template <class Tag, class... Types>
inline constexpr bool has_tag() {
  bool value = false;
  using swallow = bool[];
  (void) swallow {
    bool{},
    (value = value || std::is_same<
      Tag, named_types::__ntuple_tag_spec_t<Types>>::value)... };
  return value;
}

}  // namespace config
}  // namespace tops

#endif  // CONFIG_HAS_TAG_
