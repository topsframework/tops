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

#ifndef CONFIG_EXPLICIT_DURATION_CONFIG_
#define CONFIG_EXPLICIT_DURATION_CONFIG_

// Standard headers
#include <memory>

// Internal headers
#include "config/ConfigWithOptions.hpp"

#include "config/Options.hpp"
#include "config/model/ModelConfig.hpp"
#include "config/duration/DurationConfig.hpp"

namespace tops {
namespace config {

/**
 * @typedef ExplicitDurationConfig
 * @brief Alias to intermediate representation of a model::ExplicitDuration
 */
using ExplicitDurationConfig
  = config_with_options<
      option::Model(decltype("model"_t)),
      option::Size(decltype("max_size"_t))
    >::extending<DurationConfig>::type<class ExplicitDurationConfigID>;

/**
 * @typedef ExplicitDurationConfigPtr
 * @brief Alias of pointer to ExplicitDurationConfig
 */
using ExplicitDurationConfigPtr = std::shared_ptr<ExplicitDurationConfig>;

}  // namespace config
}  // namespace tops

#endif  // CONFIG_EXPLICIT_DURATION_CONFIG_
