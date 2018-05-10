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

#ifndef CONFIG_TRAINING_VLMC_CONFIG_
#define CONFIG_TRAINING_VLMC_CONFIG_

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
 * @typedef VLMCConfig
 * Alias to IR of the parameters of the algorithms
 * used to train a model::VariableLengthMarkovChain
 */
using VLMCConfig
  = config_with_options<>::extending<ModelConfig>::type<class VLMCConfigID>;

/**
 * @typedef VLMCConfigPtr
 * @brief Alias of pointer to VLMCConfig
 */
using VLMCConfigPtr = std::shared_ptr<VLMCConfig>;

}  // namespace training
}  // namespace config
}  // namespace tops

namespace tops {
namespace config {
namespace training {
namespace VLMC {

/**
 * @typedef ContextConfig
 * Alias to IR of the parameters of the Context algorithm,
 * used to train a model::VariableLengthMarkovChain
 */
using ContextConfig
  = config_with_options<
      option::Counter(decltype("delta"_t))
    >::extending<VLMCConfig>::type<class ContextConfigID>;

/**
 * @typedef ContextConfigPtr
 * @brief Alias of pointer to ContextConfig
 */
using ContextConfigPtr
  = std::shared_ptr<ContextConfig>;

/**
 * @typedef FixedLengthConfig
 * Alias to IR of the parameters of the Fixed Length algorithm,
 * used to train a model::VariableLengthMarkovChain
 */
using FixedLengthConfig
  = config_with_options<
      option::Model(decltype("initial_model"_t)),
      option::Dataset(decltype("weights"_t)),
      option::Order(decltype("order"_t)),
      option::Counter(decltype("pseudo_counter"_t))
    >::extending<VLMCConfig>::type<class FixedLengthConfigID>;

/**
 * @typedef FixedLengthConfigPtr
 * @brief Alias of pointer to FixedLengthConfig
 */
using FixedLengthConfigPtr
  = std::shared_ptr<FixedLengthConfig>;

/**
 * @typedef InterpolationConfig
 * Alias to IR of the parameters of the Fixed Length algorithm,
 * used to train a model::VariableLengthMarkovChain
 */
using InterpolationConfig
  = config_with_options<
      option::Model(decltype("initial_model"_t)),
      option::Dataset(decltype("weights"_t)),
      option::Order(decltype("order"_t)),
      option::Counter(decltype("pseudo_counter"_t))
    >::extending<VLMCConfig>::type<class InterpolationConfigID>;

/**
 * @typedef InterpolationConfigPtr
 * @brief Alias of pointer to InterpolationConfig
 */
using InterpolationConfigPtr
  = std::shared_ptr<InterpolationConfig>;

}  // namespace VLMC
}  // namespace training
}  // namespace config
}  // namespace tops

#endif  // CONFIG_TRAINING_VLMC_CONFIG_
