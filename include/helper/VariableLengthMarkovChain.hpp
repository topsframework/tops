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

#ifndef TOPS_HELPER_VARIABLE_LENGTH_MARKOV_CHAIN_
#define TOPS_HELPER_VARIABLE_LENGTH_MARKOV_CHAIN_

// Tested header
#include "model/VariableLengthMarkovChain.hpp"

namespace tops {
namespace helper {

tops::model::VariableLengthMarkovChainPtr generateRandomVLMC(
    size_t number_of_nodes,
    size_t alphabet_size);

tops::model::VariableLengthMarkovChainPtr createVLMCMC();
tops::model::VariableLengthMarkovChainPtr createMachlerVLMC();

}  // namespace helper
}  // namespace tops

#endif  // TOPS_HELPER_VARIABLE_LENGTH_MARKOV_CHAIN_
