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

#ifndef TOPS_APP_TRAINING_HPP
#define TOPS_APP_TRAINING_HPP

// Standard headers
#include <map>
#include <memory>
#include <string>
#include <vector>

// Internal headers
#include "app/Alphabet.hpp"

#include "model/Symbol.hpp"
#include "model/DiscreteIIDModel.hpp"
#include "model/ProbabilisticModel.hpp"

namespace tops {
namespace app {

// Globals
extern std::string DATASET;

// Auxiliaries
std::vector<std::vector<model::Symbol>>
readSequence(std::string filename, std::vector<AlphabetPtr> alphabets);

model::DiscreteIIDModelPtr trainDuration(std::string model_name,
                                         unsigned int max,
                                         unsigned int m,
                                         double slope);

// Non Coding
model::ProbabilisticModelPtr trainNonCodingModel(AlphabetPtr nucleotides);

// CDS
model::ProbabilisticModelPtr trainCDSModel(AlphabetPtr nucleotides);
model::ProbabilisticModelPtr trainRCDSModel(AlphabetPtr nucleotides);

// Stop
model::ProbabilisticModelPtr trainStopModel(AlphabetPtr nucleotides);
model::ProbabilisticModelPtr trainRStopModel(AlphabetPtr nucleotides);

// Start
model::ProbabilisticModelPtr trainStartModel(AlphabetPtr nucleotides);
model::ProbabilisticModelPtr trainRStartModel(AlphabetPtr nucleotides);

// Donor
model::ProbabilisticModelPtr trainDonorModel(AlphabetPtr nucleotides);
model::ProbabilisticModelPtr trainRDonorModel(AlphabetPtr nucleotides);

// Acceptor
model::ProbabilisticModelPtr trainAcceptorModel(AlphabetPtr nucleotides);
model::ProbabilisticModelPtr trainRAcceptorModel(AlphabetPtr nucleotides);

}  // namespace app
}  // namespace model

#endif  // TOPS_APP_TRAINING_HPP
