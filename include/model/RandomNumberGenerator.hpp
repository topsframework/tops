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

#ifndef TOPS_MODEL_RANDOM_NUMBER_GENERATOR_
#define TOPS_MODEL_RANDOM_NUMBER_GENERATOR_

// Standard headers
#include <memory>

namespace tops {
namespace model {

class RandomNumberGenerator;

/**
 * @typedef RandomNumberGeneratorPtr
 * @brief Alias of pointer to RandomNumberGenerator.
 */
using RandomNumberGeneratorPtr = std::shared_ptr<RandomNumberGenerator>;

/**
 * @class RandomNumberGenerator
 * @brief TODO
 */
class RandomNumberGenerator {
 public:
  // Alias
  using result_type = uint_fast32_t;

  // Purely Virtual methods
  virtual void seed (result_type val) = 0;
  virtual result_type operator()() = 0;
  virtual void discard (unsigned long long z) = 0;
  virtual double generateDoubleInUnitInterval() = 0;
};

}  // namespace model
}  // namespace tops

#endif
