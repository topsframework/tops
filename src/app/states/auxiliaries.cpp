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

// Interface header
#include "app/states.hpp"

// Standard headers
#include <iostream>
#include <vector>

// Internal headers
#include "model/Probability.hpp"

#include "model/GeneralizedHiddenMarkovModel.hpp"

// Imports
using tops::model::Probability;

// Aliases
using GHMM = tops::model::GeneralizedHiddenMarkovModel;

namespace tops {
namespace app {

/*----------------------------------------------------------------------------*/

std::vector<Probability> index_probabilities(
    const std::vector<GHMM::State::Id>& indices,
    const std::vector<Probability>& probabilities) {
  std::vector<Probability> indexed_probabilities(state_names.size());
  for (std::size_t i = 0; i < indices.size(); i++)
    indexed_probabilities[indices[i]] = probabilities[i];
  return indexed_probabilities;
}

/*----------------------------------------------------------------------------*/

void print_probabilities(const std::vector<Probability>& probabilities) {
  for (std::size_t i = 0; i < probabilities.size(); i++)
    std::cout << probabilities[i] << " ";
  std::cout << std::endl;
}

/*----------------------------------------------------------------------------*/

}  // namespace app
}  // namespace tops
