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

// Internal headers
#include "model/ProbabilisticModelCrtp.hpp"

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
 * According to [Aaron Clauset], IID means that if we are given a set of data
 * \f$\{x_i\}\f$, each of these \f$x_i\f$ observations is an independent choose
 * from a fixed probabilistic model. Independence means that
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
 * [Aaron Clauset]: http://tuvalu.santafe.edu/~aaronc/courses/7000/csci7000-001_2011_L0.pdf
 *
 */
class DiscreteIIDModel : public ProbabilisticModelCrtp<DiscreteIIDModel> {
 public:
  // Tags
  class maximum_likehood_algorithm {};
  class smoothed_histogram_burge_algorithm {};
  class smoothed_histogram_stanke_algorithm {};
  class smoothed_histogram_kernel_density_algorithm {};

  // Aliases
  using Self = DiscreteIIDModel;
  using SelfPtr = DiscreteIIDModelPtr;
  using Base = ProbabilisticModelCrtp<Self>;

  /*=============================[ CONSTRUCTORS ]=============================*/

  /**
   * Constructor. You should not call it directly.
   * @param probabilities Probability of each symbol be drawn by this model
   * @see trainML()
   * @see trainSmoothedHistogramBurge()
   * @see trainSmoothedHistogramStanke()
   * @see trainSmoothedHistogramKernelDensity()
   */
  explicit DiscreteIIDModel(std::vector<Probability> probabilities);

  /*============================[ STATIC METHODS ]============================*/

  // Trainer

  /**
   * Trains a new discrete iid model using the maximum likelihood method.
   * @param training_set A list of sequences used to train this model
   * @param alphabet_size \f$|X|\f$
   * @return A trained discrete iid model
   */
  static SelfPtr train(TrainerPtr<Standard, Self> trainer,
                       maximum_likehood_algorithm,
                       unsigned int alphabet_size);

  /**
   * Trains a new discrete iid model using the Smoothed Histogram Method
   * defined by [Burge].
   * @param training_set A list of sequences used to train this model
   * @param c TODO
   * @param max_length TODO
   * @return A trained discrete iid model
   */
  static SelfPtr train(TrainerPtr<Standard, Self> trainer,
                       smoothed_histogram_burge_algorithm,
                       double c,
                       unsigned int max_length);

  /**
   * Trains a new discrete iid model using the Smoothed Histogram Method
   * defined by [Stanke].
   * @param training_set A list of sequences used to train this model
   * @param weights a list of weights of each sequence in training_set
   * @param max_length TODO
   * @param m TODO
   * @param slope TODO
   * @return A trained discrete iid model
   */
  static SelfPtr train(TrainerPtr<Standard, Self> trainer,
                       smoothed_histogram_stanke_algorithm,
                       std::vector<unsigned int> weights,
                       unsigned int max_length,
                       unsigned int m,
                       double slope);

  /**
   * Trains a new discrete iid model using the Smoothed Histogram Kernel
   * Density Method defined by [Sheather].
   * @param training_set A list of sequences used to train this model
   * @param max_length TODO
   * @return A trained discrete iid model
   */
  static SelfPtr train(TrainerPtr<Standard, Self> trainer,
                       smoothed_histogram_kernel_density_algorithm,
                       unsigned int max_length);

  // Others

  /**
   * Normalizes values into probabilities.
   * @param probabilities A vector of real values
   * @return A vector of probabilities, normalized accordingly to the mean
   */
  static std::vector<Probability> normalize(std::vector<double> values);

  /*==========================[ OVERRIDEN METHODS ]===========================*/
  /*-------------------------( Probabilistic Model )--------------------------*/

  // StandardEvaluator
  Probability evaluateSymbol(SEPtr<Standard> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(SEPtr<Standard> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  // CachedEvaluator
  void initializeCache(CEPtr<Standard> evaluator,
                       unsigned int phase) override;
  Probability evaluateSymbol(CEPtr<Standard> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(CEPtr<Standard> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  // StandardGenerator
  Standard<Symbol> drawSymbol(SGPtr<Standard> generator,
                              unsigned int pos,
                              unsigned int phase,
                              const Sequence& context) const override;
  Standard<Sequence> drawSequence(SGPtr<Standard> generator,
                                  unsigned int size,
                                  unsigned int phase) const override;

  // SimpleSerializer
  void serialize(SSPtr serializer) override;

  /*===========================[ VIRTUAL METHODS ]============================*/

  /**
   * Draws a new symbol.
   * @return \f$x,\ x \in X\f$
   */
  virtual Symbol draw(RandomNumberGeneratorPtr rng) const;

  /**
   * Gets the probability of this model drawing a given symbol.
   * @return \f$Pr(s)\f$
   */
  virtual Probability probabilityOf(Symbol s) const;

  /*==========================[ CONCRETE METHODS ]============================*/

  /**
   * Gets a vector with probabilities of this model drawing each symbol
   * of its alphabet.
   * @return \f$\{Pr(x)\},\ x \in X\f$
   */
  std::vector<Probability> probabilities();

  /**
   * Gets the model's alphabet size.
   * @return \f$|X|\f$
   */
  int alphabetSize() const;

 private:
  // Instance variables
  std::vector<Probability> _probabilities;

  /*============================[ STATIC METHODS ]============================*/

  static double kernel_normal(double x, double h);
  static double epanechnikov(double x, double h);
  static void band_den_bin(int n,
                           int nb,
                           double* d,
                           const std::vector<double>& x,
                           std::vector<double>& cnt);
  static void band_phi6_bin(int n,
                            int nb,
                            double d,
                            std::vector<double>& x,
                            double h,
                            double* u);
  static void band_phi4_bin(int n,
                            int nb,
                            double d,
                            std::vector<double> x,
                            double h,
                            double* u);
  static double mean(const std::vector<double>& data);
  static double var(const std::vector<double>& data);
  static double quantile(std::vector<double> data, double q);
  static double iqr(const std::vector<double>& data);
  static double kernel_density_estimation(double x,
                                          double bw,
                                          const std::vector<double>& data);
  static double sj_bandwidth(const std::vector<double>& data);
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_DISCRETE_IID_MODEL_
