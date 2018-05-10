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

#ifndef CONFIG_TRAINING_PERIODIC_IMC_CONFIG_
#define CONFIG_TRAINING_PERIODIC_IMC_CONFIG_

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
 * @typedef PeriodicIMCConfig
 * Alias to IR of the parameters of the algorithms
 * used to train a model::PeriodicInhomogeneousMarkovChain
 */
using PeriodicIMCConfig
  = config_with_options<
    >::extending<ModelConfig>::type<class PeriodicIMCConfigID>;

/**
 * @typedef PeriodicIMCConfigPtr
 * @brief Alias of pointer to PeriodicIMCConfig
 */
using PeriodicIMCConfigPtr = std::shared_ptr<PeriodicIMCConfig>;

}  // namespace training
}  // namespace config
}  // namespace tops

namespace tops {
namespace config {
namespace training {
namespace PeriodicIMC {

/**
 * @typedef InterpolationConfig
 * Alias to IR of the parameters of the Interpolation algorithm,
 * used to train a model::PeriodicInhomogeneousMarkovChain
 */
using InterpolationConfig
  = config_with_options<
      option::Model(decltype("initial_model"_t)),
      option::Dataset(decltype("weights"_t)),
      option::Order(decltype("order"_t)),
      option::Order(decltype("number_of_phases"_t)),
      option::Counter(decltype("pseudo_counter"_t))
    >::extending<PeriodicIMCConfig>::type<class InterpolationConfigID>;

/**
 * @typedef InterpolationConfigPtr
 * @brief Alias of pointer to InterpolationConfig
 */
using InterpolationConfigPtr
  = std::shared_ptr<InterpolationConfig>;

}  // namespace PeriodicIMC
}  // namespace training
}  // namespace config
}  // namespace tops

#endif  // CONFIG_TRAINING_PERIODIC_IMC_CONFIG_
