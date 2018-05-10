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

#ifndef CONFIG_DEFINITION_IMC_CONFIG_
#define CONFIG_DEFINITION_IMC_CONFIG_

// Standard headers
#include <memory>

// Internal headers
#include "config/ConfigWithOptions.hpp"

#include "config/model/definition/ModelConfig.hpp"

namespace tops {
namespace config {
namespace definition {

/**
 * @typedef IMCConfig
 * @brief Alias to IR of a model::InhomogeneousMarkovChain
 */
using IMCConfig
  = config_with_options<
      option::Models(decltype("position_specific_distributions"_t))
    >::extending<ModelConfig>::type<class IMCConfigID>;

/**
 * @typedef IMCConfigPtr
 * @brief Alias of pointer to IMCConfig
 */
using IMCConfigPtr = std::shared_ptr<IMCConfig>;

}  // namespace definition
}  // namespace config
}  // namespace tops

#endif  // CONFIG_DEFINITION_IMC_CONFIG_
