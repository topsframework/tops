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

#ifndef TOPS_MODEL_DECODABLE_MODEL_
#define TOPS_MODEL_DECODABLE_MODEL_

#include <memory>

// ToPS headers
#include "model/ProbabilisticModel.hpp"
#include "model/Matrix.hpp"
#include "model/Labeling.hpp"
#include "model/DecodableEvaluator.hpp"

namespace tops {
namespace model {

/**
 * @class DecodableModel
 * @brief TODO
 */
class DecodableModel : public ProbabilisticModel {
 public:
  // Alias
  struct Cache {
    std::vector<double> prefix_sum_array;
    Matrix alpha, beta, gamma, posterior_decoding;
  };

  // Virtual methods
  virtual EvaluatorPtr evaluator(const Sequence &s,
                                 bool cached = false) = 0;
  virtual DecodableEvaluatorPtr decodableEvaluator(const Sequence &s,
                                                   bool cached = false) = 0;

  virtual double evaluate(const Sequence &context,
                          unsigned int pos,
                          unsigned int phase = 0) const = 0;
  virtual double evaluate(const Sequence &xs,
                          const Sequence &ys,
                          unsigned int i) const = 0;

  virtual void chooseSequences(Sequence &xs,
                               Sequence &ys,
                               unsigned int size) const = 0;
  virtual void chooseSequencesPosition(Sequence &xs,
                                       Sequence &ys,
                                       unsigned int i) const = 0;

  virtual double backward(const Sequence &s,
                          Matrix &beta) const = 0;
  virtual double forward(const Sequence &s,
                         Matrix &alpha) const = 0;
  virtual void posteriorProbabilities(const Sequence &xs,
                                      Matrix &probabilities) const = 0;

  virtual Labeling labeling(const Sequence &xs,
                            Matrix &probabilities,
                            Labeling::Method method) const = 0;

 private:
  // Virtual methods
  virtual Labeling viterbi(const Sequence &xs,
                           Matrix &gamma) const = 0;
  virtual Labeling posteriorDecoding(const Sequence &xs,
                                     Matrix &probabilities) const = 0;
};

/**
 * @typedef DecodableModelPtr
 * @brief Alias of pointer to DecodableModel.
 */
using DecodableModelPtr = std::shared_ptr<DecodableModel>;

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_DECODABLE_MODEL_
