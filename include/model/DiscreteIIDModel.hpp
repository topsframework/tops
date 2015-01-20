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

#ifndef DISCRETE_IID_MODEL_HPP_
#define DISCRETE_IID_MODEL_HPP_

#include <memory>
#include <vector>

#include "FactorableModel.hpp"

namespace tops {
namespace model {

class DiscreteIIDModel : public FactorableModel {
 public:
  explicit DiscreteIIDModel(const std::vector<double> &probabilities);

  virtual int alphabetSize();

  virtual double choose() const;
  virtual Symbol choosePosition(const Sequence &sequence,
                                unsigned int index) const;

  virtual double logProbabilityOf(Symbol symbol);
  virtual double evaluate(const Sequence &sequence,
                          unsigned int begin,
                          unsigned int end);
  virtual double evaluatePosition(const Sequence &sequence,
                                  unsigned int index) const;
};

typedef std::shared_ptr<DiscreteIIDModel> DiscreteIIDModelPtr;

}  // namespace model
}  // namespace tops

#endif
