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

#ifndef TOPS_MODEL_PHASED_RUN_LENGTH_DISTRIBUTION_
#define TOPS_MODEL_PHASED_RUN_LENGTH_DISTRIBUTION_

// Standard headers
#include <memory>
#include <vector>

// ToPS headers
#include "model/DiscreteIIDModel.hpp"

namespace tops {
namespace model {

class PhasedRunLengthDistribution;
using PhasedRunLengthDistributionPtr = std::shared_ptr<PhasedRunLengthDistribution>;

class PhasedRunLengthDistribution : public DiscreteIIDModel {
 public:
  // Static methods
  static PhasedRunLengthDistributionPtr make(std::vector<double> probabilities, int delta, int input_phase, int output_phase, int nphase);
  static PhasedRunLengthDistributionPtr makeFromDiscreteIIDModel(DiscreteIIDModelPtr model, int delta, int input_phase, int output_phase, int nphase);

  // Virtual methods
  virtual double probabilityOf(Symbol s) const;
  virtual Symbol choose() const;

 private:
  // Instance variables
  int _delta;
  int _input_phase;
  int _output_phase;
  int _nphase;
  double _normfactor;
  std::vector<double> _probabilities;

  // Constructors
  PhasedRunLengthDistribution(std::vector<double> probabilities, int delta, int input_phase, int output_phase, int nphase);

  // Instance methods
  int mod(int D, int d) const;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_PHASED_RUN_LENGTH_DISTRIBUTION_
