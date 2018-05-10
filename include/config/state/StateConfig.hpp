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

#ifndef CONFIG_STATE_CONFIG_
#define CONFIG_STATE_CONFIG_

// Standard headers
#include <map>
#include <memory>
#include <string>
#include <vector>

// Internal headers
#include "config/ConfigWithOptions.hpp"

#include "config/Options.hpp"
#include "config/model/ModelConfig.hpp"
#include "config/duration/DurationConfig.hpp"

namespace tops {
namespace config {

/**
 * @typedef StateConfig
 * @brief Alias to intermediate representation of a model::State
 */
using StateConfig
  = config_with_options<
      option::Duration(decltype("duration"_t)),
      option::Model(decltype("emission"_t))
    >::type<class StateConfigID>;

/**
 * @typedef StateConfigPtr
 * @brief Alias of pointer to StateConfig
 */
using StateConfigPtr = std::shared_ptr<StateConfig>;

}  // namespace config
}  // namespace tops

namespace tops {
namespace config {
namespace option {

using State = config::StateConfigPtr;
using States = std::map<std::string, State>;

}  // namespace option
}  // namespace config
}  // namespace tops

#endif  // CONFIG_STATE_CONFIG_
