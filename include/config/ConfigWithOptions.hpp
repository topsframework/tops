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

#ifndef CONFIG_CONFIG_WITH_OPTIONS_
#define CONFIG_CONFIG_WITH_OPTIONS_

// Internal headers
#include "config/Util.hpp"
#include "config/StringLiteralSuffix.hpp"

namespace tops {
namespace config {

// Forward declaration
class Config;
template<typename ID, typename Base, typename... Options> class BasicConfig;

/**
 * @brief Metafunction to create BasicConfig
 * @tparam Options List of Type(decltype("name"_t)) to compose BasicConfig
 */
template<typename... Options>
struct config_with_options {
  template<typename ID>
  using type = BasicConfig<ID, Config, Options...>;

  template<typename T>
  struct extending {
    static_assert(delayed_false<T>::value, "Is not a configuration");
  };

  template<typename BaseID, typename BaseBase, typename... BaseOptions>
  struct extending<BasicConfig<BaseID, BaseBase, BaseOptions...>> {
    using Base = BasicConfig<BaseID, BaseBase, BaseOptions...>;

    template<typename ID>
    using type = BasicConfig<ID, Base, Options...>;
  };
};

}  // namespace config
}  // namespace tops

#endif  // CONFIG_CONFIG_WITH_OPTIONS_
