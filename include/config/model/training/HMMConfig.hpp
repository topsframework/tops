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

#ifndef CONFIG_TRAINING_HMM_CONFIG_
#define CONFIG_TRAINING_HMM_CONFIG_

// Standard headers
#include <memory>

// Internal headers
#include "config/ConfigWithOptions.hpp"

#include "config/Options.hpp"
#include "config/model/training/DecodableModelConfig.hpp"

namespace tops {
namespace config {
namespace training {

/**
 * @typedef HMMConfig
 * Alias to IR of the parameters of the algorithms
 * used to train a model::HiddenMarkovModel
 */
using HMMConfig
  = config_with_options<>::extending<DecodableModelConfig>
                         ::type<class HMMConfigID>;

/**
 * @typedef HMMConfigPtr
 * @brief Alias of pointer to HMMConfig
 */
using HMMConfigPtr = std::shared_ptr<HMMConfig>;

}  // namespace training
}  // namespace config
}  // namespace tops

namespace tops {
namespace config {
namespace training {
namespace HMM {

/**
 * @typedef MaximumLikehoodConfig
 * Alias to IR of the parameters of the Maximum Likehood algorithm,
 * used to train a model::HiddenMarkovModel
 */
using MaximumLikehoodConfig
  = config_with_options<
      option::Counter(decltype("pseudo_counter"_t))
    >::extending<HMMConfig>::type<class MaximumLikehoodConfigID>;

/**
 * @typedef MaximumLikehoodConfigPtr
 * @brief Alias of pointer to MaximumLikehoodConfig
 */
using MaximumLikehoodConfigPtr
  = std::shared_ptr<MaximumLikehoodConfig>;

/**
 * @typedef BaumWelchConfig
 * Alias to IR of the parameters of the Baum-Welch algorithm,
 * used to train a model::HiddenMarkovModel
 */
using BaumWelchConfig
  = config_with_options<
      option::Model(decltype("initial_model"_t)),
      option::Iterations(decltype("maximum_iterations"_t)),
      option::Threshold(decltype("diff_threshold"_t))
    >::extending<HMMConfig>::type<class BaumWelchConfigID>;

/**
 * @typedef BaumWelchConfigPtr
 * @brief Alias of pointer to BaumWelchConfig
 */
using BaumWelchConfigPtr
  = std::shared_ptr<BaumWelchConfig>;

}  // namespace HMM
}  // namespace training
}  // namespace config
}  // namespace tops

#endif  // CONFIG_TRAINING_HMM_CONFIG_
