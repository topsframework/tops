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
 *        distributed (IID) model.
 *
 * According to [Aaron Clauset](http://tuvalu.santafe.edu/~aaronc/courses/7000/csci7000-001_2011_L0.pdf), IID means that if we are given a set of data \f$\{x_i\}\f$, each of these
 * \f$x_i\f$ observations is an independent draw from a fixed probabilistic
 * model. Independence means that
 *
 * \f[
 * Pr(x_1)\ and\ Pr(x_2) = Pr(x_1) Pr(x_2)
 * \f]
 *
 * That is, the probability of observing two values \f$x_1\f$ and \f$x_2\f$ is
 * simply the probability of observing \f$x_1\f$ multiplied by the probability
 * we observe \f$x_2\f$. This implies what’s called conditional independence,
 * that is,
 *
 * \f[
 * Pr(x_2 | x_1) = Pr(x_2)
 * \f]
 *
 * To give you some intuition about what this means, consider the familiar
 * process of rolling dice. If we assume that we have 2 regular 6-sided dice
 * and that they are “fair,” then each of the 6 values occurs with equal
 * probability. If we throw the dice together, the values they display are iid
 * random variables. If we throw the dice separately, the values they display
 * are iid random variables. If we throw a long sequence of the dice, all of
 * the values we observe are iid random variables.
 *
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
  static DiscreteIIDModelPtr trainSmoothedHistogramKernelDensity(
      std::vector<Sequence> training_set,
      unsigned int max_length);

  // Virtual methods
  virtual int alphabetSize() const;
  virtual double evaluatePosition(const Sequence &s, unsigned int i) const;
  virtual Symbol choosePosition(const Sequence &s, unsigned int i) const;
  virtual Symbol choose() const;
  virtual double probabilityOf(Symbol s) const;

  // Concrete methods
  std::vector<double> probabilities();

 protected:
  // Constructors
  explicit DiscreteIIDModel(std::vector<double> probabilities);

 private:
  // Instance variables
  std::vector<double> _probabilities;

  // Static methods
  static std::vector<double> normalize(std::vector<double> probabilities);
  static double kernel_normal(double x, double h);
  static double epanechnikov(double x, double h);
  static void band_den_bin(int n,
                           int nb,
                           double *d,
                           const std::vector<double> &x,
                           std::vector<double> &cnt);
  static void band_phi6_bin(int n,
                            int nb,
                            double d,
                            std::vector<double> &x,
                            double h,
                            double *u);
  static void band_phi4_bin(int n,
                            int nb,
                            double d,
                            std::vector<double> x,
                            double h,
                            double *u);
  static double mean(const std::vector<double> &data);
  static double var(const std::vector<double> &data);
  static double quantile(std::vector<double> data, double q);
  static double iqr(const std::vector<double> &data);
  static double kernel_density_estimation(double x,
                                          double bw,
                                          const std::vector<double> &data);
  static double sj_bandwidth(const std::vector<double> &data);
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_DISCRETE_IID_MODEL_
