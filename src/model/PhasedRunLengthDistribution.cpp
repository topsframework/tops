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
#include "model/PhasedRunLengthDistribution.hpp"

// Standard headers
#include <cmath>
#include <limits>
#include <vector>

// Internal headers
#include "model/Util.hpp"

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

PhasedRunLengthDistribution::PhasedRunLengthDistribution(
    std::vector<Probability> probabilities,
    int delta,
    int input_phase,
    int output_phase,
    int nphase) : DiscreteIIDModel(probabilities),
                  _delta(delta),
                  _input_phase(input_phase),
                  _output_phase(output_phase),
                  _nphase(nphase) {
  Probability sum = 0;
  for (int i = 0; i < alphabetSize(); i++) {
    int d = i + _delta;
    if (mod((d + _input_phase-1), _nphase) == _output_phase)
      sum += DiscreteIIDModel::probabilityOf(i);
  }
  _normfactor = sum;
}

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

PhasedRunLengthDistributionPtr PhasedRunLengthDistribution::make(
    std::vector<Probability> probabilities,
    int delta,
    int input_phase,
    int output_phase,
    int nphase) {
  return PhasedRunLengthDistributionPtr(
    new PhasedRunLengthDistribution(probabilities,
                                    delta,
                                    input_phase,
                                    output_phase,
                                    nphase));
}

/*----------------------------------------------------------------------------*/

PhasedRunLengthDistributionPtr
  PhasedRunLengthDistribution::makeFromDiscreteIIDModel(
    DiscreteIIDModelPtr model,
    int delta,
    int input_phase,
    int output_phase,
    int nphase) {
  return make(model->probabilities(),
              delta,
              input_phase,
              output_phase,
              nphase);
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

Probability PhasedRunLengthDistribution::probabilityOf(Symbol s) const {
  int d = s + _delta;
  if (mod((d + _input_phase-1), _nphase) != _output_phase) return 0;
  Probability result = DiscreteIIDModel::probabilityOf(d);
  return result / _normfactor;
}

/*----------------------------------------------------------------------------*/

Symbol PhasedRunLengthDistribution::draw(RandomNumberGeneratorPtr rng) const {
  int L = static_cast<int>(DiscreteIIDModel::draw(rng));
  while (mod((L + _input_phase-1), _nphase ) != _output_phase)
    L++;

  int d = L - _delta;
  if (d < 1) {
    L = _delta+1;
    while (mod((L + _input_phase-1), _nphase ) != _output_phase)
      L++;
    d = L - _delta;
  }

  return d;
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

int PhasedRunLengthDistribution::mod(int D, int d) const {
  int r = D%d;
  if (r < 0) {
    if (d > 0)
      r = r + d;
    else
      r = r - d;
  }
  return r;
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
