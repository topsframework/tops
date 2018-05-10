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

#ifndef CONFIG_DEFINITION_GHMM_CONFIG_
#define CONFIG_DEFINITION_GHMM_CONFIG_

// Standard headers
#include <memory>

// Internal headers
#include "config/ConfigWithOptions.hpp"

#include "config/Options.hpp"
#include "config/state/StateConfig.hpp"
#include "config/model/definition/DecodableModelConfig.hpp"

namespace tops {
namespace config {
namespace definition {

/**
 * @typedef GHMMConfig
 * @brief Alias to IR of a model::GeneralizedHiddenMarkovModel
 */
using GHMMConfig
  = config_with_options<
      option::Probabilities(decltype("transition_probabilities"_t)),
      option::States(decltype("states"_t))
    >::extending<DecodableModelConfig>::type<class GHMMConfigID>;

/**
 * @typedef GHMMConfigPtr
 * @brief Alias of pointer to GHMMConfig
 */
using GHMMConfigPtr = std::shared_ptr<GHMMConfig>;

}  // namespace definition
}  // namespace config
}  // namespace tops

#endif  // CONFIG_DEFINITION_GHMM_CONFIG_
