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

#ifndef CONFIG_TRAINING_DECODABLE_MODEL_CONFIG_
#define CONFIG_TRAINING_DECODABLE_MODEL_CONFIG_

// Standard headers
#include <memory>
#include <vector>

// Internal headers
#include "config/ConfigWithOptions.hpp"

#include "config/Domain.hpp"
#include "config/Options.hpp"
#include "config/model/training/ModelConfig.hpp"

namespace tops {
namespace config {
namespace training {

/**
 * @typedef DecodableModelConfig
 * @brief Alias to IR of a model::ProbabilisticDecodableModel
 */
using DecodableModelConfig
  = config_with_options<
      option::Domains(decltype("other_observations"_t)),
      option::Domain(decltype("labels"_t))
    >::extending<ModelConfig>::type<class DecodableModelConfigID>;

/**
 * @typedef DecodableModelConfigPtr
 * @brief Alias of pointer to DecodableModelConfig
 */
using DecodableModelConfigPtr = std::shared_ptr<DecodableModelConfig>;

}  // namespace training
}  // namespace config
}  // namespace tops

#endif  // CONFIG_TRAINING_DECODABLE_MODEL_CONFIG_
