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

#ifndef TOPS_MODEL_DISCRETE_IID_MODEL_
#define TOPS_MODEL_DISCRETE_IID_MODEL_

// Standard headers
#include <memory>
#include <vector>

// ToPS headers
#include "model/FactorableModel.hpp"

namespace tops {
namespace model {

class DiscreteIIDModel;
using DiscreteIIDModelPtr = std::shared_ptr<DiscreteIIDModel>;

class DiscreteIIDModel : public FactorableModel {
 public:
  static DiscreteIIDModelPtr make(std::vector<double> probabilities);

  virtual int alphabetSize() const;
  virtual double evaluatePosition(const Sequence &s, unsigned int i) const;
  virtual int choosePosition(const Sequence &s, unsigned int i) const;

  double probabilityOf(Symbol s) const;

 private:
  explicit DiscreteIIDModel(std::vector<double> probabilities);

  std::vector<double> _probabilities;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_DISCRETE_IID_MODEL_
