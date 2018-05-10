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

#ifndef CONFIG_TRAINING_MDD_CONFIG_
#define CONFIG_TRAINING_MDD_CONFIG_

// Standard headers
#include <memory>

// Internal headers
#include "config/ConfigWithOptions.hpp"

#include "config/Options.hpp"
#include "config/model/training/ModelConfig.hpp"

namespace tops {
namespace config {
namespace training {

/**
 * @typedef MDDConfig
 * Alias to IR of the parameters of the algorithms
 * used to train a model::MaximalDependenceDecomposition
 */
using MDDConfig
  = config_with_options<>::extending<ModelConfig>::type<class MDDConfigID>;

/**
 * @typedef MDDConfigPtr
 * @brief Alias of pointer to MDDConfig
 */
using MDDConfigPtr = std::shared_ptr<MDDConfig>;

}  // namespace training
}  // namespace config
}  // namespace tops

namespace tops {
namespace config {
namespace training {
namespace MDD {

/**
 * @typedef StandardConfig
 * Alias to IR of the parameters of the Maximum Likehood algorithm,
 * used to train a model::HiddenMarkovModel
 */
using StandardConfig
  = config_with_options<
      option::Pattern(decltype("consensus"_t)),
      option::Model(decltype("consensus_model"_t)),
      option::Size(decltype("minimum_subset"_t))
    >::extending<MDDConfig>::type<class StandardConfigID>;

/**
 * @typedef StandardConfigPtr
 * @brief Alias of pointer to StandardConfig
 */
using StandardConfigPtr
  = std::shared_ptr<StandardConfig>;

}  // namespace MDD
}  // namespace training
}  // namespace config
}  // namespace tops

#endif  // CONFIG_TRAINING_MDD_CONFIG_
