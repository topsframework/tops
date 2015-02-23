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

// Standard headers
#include <cmath>
#include <vector>

// ToPS headers
#include "helper/DiscreteIIDModel.hpp"
#include "helper/Random.hpp"

namespace tops {
namespace helper {

tops::model::DiscreteIIDModelPtr generateRandomIIDModel(int alphabet_size) {
  std::vector<double> counts;
  for (int i = 0; i < alphabet_size; i++)
    counts.push_back(static_cast<double>(generateRandomInteger(alphabet_size)));
  return tops::model::DiscreteIIDModel::make(counts);
}

tops::model::DiscreteIIDModelPtr createFairCoinIIDModel() {
  return tops::model::DiscreteIIDModel::make({log(0.5), log(0.5)});
}

tops::model::DiscreteIIDModelPtr createLoadedCoinIIDModel() {
  return tops::model::DiscreteIIDModel::make({log(0.2), log(0.8)});
}

tops::model::DiscreteIIDModelPtr createDNAModel() {
  return tops::model::DiscreteIIDModel::make(
      {log(0.1), log(0.3), log(0.4), log(0.2)});
}

}  // namespace helper
}  // namespace tops
