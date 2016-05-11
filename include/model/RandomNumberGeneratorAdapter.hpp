/***********************************************************************/
/*  Copyright 2015 ToPS                                                */
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

#ifndef TOPS_MODEL_RANDOM_NUMBER_GENERATOR_ADAPTER_
#define TOPS_MODEL_RANDOM_NUMBER_GENERATOR_ADAPTER_

// Standard headers
#include <memory>
#include <random>

// Internal headers
#include "model/RandomNumberGenerator.hpp"

namespace tops {
namespace model {

// Forward declaration
template<typename RNG>
class RandomNumberGeneratorAdapter;

/**
 * @typedef RandomNumberGeneratorAdapterPtr
 * @brief Alias of pointer to RandomNumberGeneratorAdapter.
 */
template<typename RNG>
using RandomNumberGeneratorAdapterPtr
  = std::shared_ptr<RandomNumberGeneratorAdapter<RNG>>;

/**
 * @class RandomNumberGeneratorAdapter
 * @brief TODO
 */
template<typename RNG>
class RandomNumberGeneratorAdapter : public RandomNumberGenerator {
 public:
  // Alias
  using Self = RandomNumberGeneratorAdapter<RNG>;
  using SelfPtr = std::shared_ptr<Self>;

  // Static methods
  template<typename... Ts>
  static RandomNumberGeneratorAdapterPtr<RNG> make(Ts... args) {
    return std::shared_ptr<Self>(new Self(std::forward<Ts>(args)...));
  }

  // Overriden methods
  void seed(result_type val = RNG::default_seed) override {
    _generator.seed(val);
  }

  result_type operator()() override {
    return _generator();
  }

  void discard(uint64_t z) override {
    _generator.discard(z);
  }

  double generateDoubleInUnitInterval() override {
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(_generator);
  }

 protected:
  // Instance variables
  RNG _generator;

  // Constructors
  template<typename... Ts>
  explicit RandomNumberGeneratorAdapter(Ts... args)
      : _generator(std::forward<Ts>(args)...) {
  }
};

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                      ALIAS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/**
 * @typedef RNGAdapter
 * @brief Alias of RandomNumberGeneratorAdapter.
 */
template<typename RNG>
using RNGAdapter = RandomNumberGeneratorAdapter<RNG>;

/**
 * @typedef RNGAdapterPtr
 * @brief Alias of RandomNumberGeneratorAdapterPtr.
 */
template<typename RNG>
using RNGAdapterPtr = RandomNumberGeneratorAdapterPtr<RNG>;

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_RANDOM_NUMBER_GENERATOR_ADAPTER_
