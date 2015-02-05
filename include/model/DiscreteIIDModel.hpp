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

/**
 * @typedef DiscreteIIDModelPtr
 * @brief Alias of pointer to DiscreteIIDModel.
 */
using DiscreteIIDModelPtr = std::shared_ptr<DiscreteIIDModel>;

/**
 * @class DiscreteIIDModel
 * @brief Class that represents an independent and identically
 *        distributed model.
 */
class DiscreteIIDModel : public FactorableModel {
 public:
  // Static methods
  static DiscreteIIDModelPtr make(std::vector<double> probabilities);
  static DiscreteIIDModelPtr trainML(std::vector<Sequence> training_set,
                                     unsigned int alphabet_size);
  static DiscreteIIDModelPtr trainSmoothedHistogramBurge(
      std::vector<Sequence> training_set,
      double c,
      unsigned int max_length);
  static DiscreteIIDModelPtr trainSmoothedHistogramStanke(
      std::vector<Sequence> training_set,
      std::vector<unsigned int> weights,
      unsigned int max_length,
      int m,
      double slope);

  // Virtual methods
  virtual int alphabetSize() const;
  virtual double evaluatePosition(const Sequence &s, unsigned int i) const;
  virtual Symbol choosePosition(const Sequence &s, unsigned int i) const;

  // Concrete methods
  double probabilityOf(Symbol s) const;

 private:
  // Instance variables
  std::vector<double> _probabilities;

  // Constructors
  explicit DiscreteIIDModel(std::vector<double> probabilities);

  // Static methods
  static double kernel_normal(double x, double h);
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_DISCRETE_IID_MODEL_
