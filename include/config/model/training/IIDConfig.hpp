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

#ifndef CONFIG_TRAINING_IID_CONFIG_
#define CONFIG_TRAINING_IID_CONFIG_

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
 * @typedef IIDConfig
 * Alias to IR of the parameters of the algorithms
 * used to train a model::DiscreteIIDModel
 */
using IIDConfig
  = config_with_options<>::extending<ModelConfig>::type<class IIDConfigID>;

/**
 * @typedef IIDConfigPtr
 * @brief Alias of pointer to IIDConfig
 */
using IIDConfigPtr = std::shared_ptr<IIDConfig>;

}  // namespace training
}  // namespace config
}  // namespace tops

namespace tops {
namespace config {
namespace training {
namespace IID {

/**
 * @typedef MaximumLikehoodConfig
 * Alias to IR of the parameters of the Maximum Likehood algorithm,
 * used to train a model::DiscreteIIDModel
 */
using MaximumLikehoodConfig
  = config_with_options<>::extending<IIDConfig>
                         ::type<class MaximumLikehoodConfigID>;

/**
 * @typedef MaximumLikehoodConfigPtr
 * @brief Alias of pointer to MaximumLikehoodConfig
 */
using MaximumLikehoodConfigPtr
  = std::shared_ptr<MaximumLikehoodConfig>;

/**
 * @typedef SmoothedHistogramBurgeConfig
 * Alias to IR of the parameters of the Smoothed Histrogram algorithm,
 * made by Burge and used to train a model::DiscreteIIDModel
 */
using SmoothedHistogramBurgeConfig
  = config_with_options<
      option::Counter(decltype("c"_t)),
      option::Length(decltype("maximum_length"_t))
    >::extending<IIDConfig>::type<class SmoothedHistogramBurgeConfigID>;

/**
 * @typedef SmoothedHistogramBurgeConfigPtr
 * @brief Alias of pointer to SmoothedHistogramBurgeConfig
 */
using SmoothedHistogramBurgeConfigPtr
  = std::shared_ptr<SmoothedHistogramBurgeConfig>;

/**
 * @typedef SmoothedHistogramStankeConfig
 * Alias to IR of the parameters of the Smoothed Histrogram algorithm,
 * made by Stanke and used to train a model::DiscreteIIDModel
 */
using SmoothedHistogramStankeConfig
  = config_with_options<
      option::Dataset(decltype("weights"_t)),
      option::Length(decltype("maximum_length"_t)),
      option::Length(decltype("m"_t)),
      option::Counter(decltype("slope"_t))
    >::extending<IIDConfig>::type<class SmoothedHistogramStankeConfigID>;

/**
 * @typedef SmoothedHistogramStankeConfigPtr
 * @brief Alias of pointer to SmoothedHistogramStankeConfig
 */
using SmoothedHistogramStankeConfigPtr
  = std::shared_ptr<SmoothedHistogramStankeConfig>;

}  // namespace IID
}  // namespace training
}  // namespace config
}  // namespace tops

#endif  // CONFIG_TRAINING_IID_CONFIG_
