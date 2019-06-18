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

#ifndef TOPS_MODEL_SIMILARITY_BASED_SEQUENCE_WEIGHTING_
#define TOPS_MODEL_SIMILARITY_BASED_SEQUENCE_WEIGHTING_

// Standard headers
#include <map>
#include <memory>
#include <vector>
#include <utility>

// Internal headers
#include "model/ProbabilisticModel.hpp"
#include "model/ProbabilisticModelCrtp.hpp"

namespace tops {
namespace model {

// Forward declaration
class SimilarityBasedSequenceWeighting;

/**
 * @typedef SimilarityBasedSequenceWeightingPtr
 * @brief Alias of pointer to SimilarityBasedSequenceWeighting.
 */
using SimilarityBasedSequenceWeightingPtr
    = std::shared_ptr<SimilarityBasedSequenceWeighting>;

/**
 * @class SimilarityBasedSequenceWeighting
 * @brief TODO
 */
class SimilarityBasedSequenceWeighting
    : public ProbabilisticModelCrtp<SimilarityBasedSequenceWeighting> {
 public:
  // Tags
  class standard_training_algorithm {};

  // Aliases
  using Self = SimilarityBasedSequenceWeighting;
  using SelfPtr = SimilarityBasedSequenceWeightingPtr;
  using Base = ProbabilisticModelCrtp<Self>;

  /*=============================[ CONSTRUCTORS ]=============================*/

  SimilarityBasedSequenceWeighting(std::map<Sequence, size_t> counter,
                                   double normalizer,
                                   size_t skip_offset,
                                   size_t skip_length,
                                   Sequence skip_sequence);

  /*============================[ STATIC METHODS ]============================*/

  // Trainer
  static SelfPtr train(TrainerPtr<Multiple, Self> trainer,
                       standard_training_algorithm,
                       size_t alphabet_size,
                       size_t skip_offset,
                       size_t skip_length,
                       Sequence skip_sequence);

  /*==========================[ OVERRIDEN METHODS ]===========================*/
  /*-------------------------( Probabilistic Model )--------------------------*/

  // StandardEvaluator
  Probability evaluateSymbol(SEPtr<Multiple> evaluator,
                             size_t pos,
                             size_t phase) const override;
  Probability evaluateSequence(SEPtr<Multiple> evaluator,
                               size_t begin,
                               size_t end,
                               size_t phase) const override;

  // CachedEvaluator
  void initializeCache(CEPtr<Multiple> evaluator,
                       size_t phase) override;
  Probability evaluateSymbol(CEPtr<Multiple> evaluator,
                             size_t pos,
                             size_t phase) const override;
  Probability evaluateSequence(CEPtr<Multiple> evaluator,
                               size_t begin,
                               size_t end,
                               size_t phase) const override;

  // StandardGenerator
  Multiple<Symbol> drawSymbol(SGPtr<Multiple> generator,
                              size_t pos,
                              size_t phase,
                              const Multiple<Sequence>& context) const override;
  Multiple<Sequence> drawSequence(SGPtr<Multiple> generator,
                                  size_t size,
                                  size_t phase) const override;

  // SimpleSerializer
  void serialize(SSPtr serializer) override;

 private:
  // Instance variables
  std::map<Sequence, size_t> _counter;
  size_t _skip_offset;
  size_t _skip_length;
  Sequence _skip_sequence;
  double _normalizer;

  /*============================[ STATIC METHODS ]============================*/

  static double
  calculate_normalizer(size_t skip_length,
                       size_t skip_offset,
                       size_t max_length,
                       const std::map<Sequence, size_t>& counter,
                       size_t alphabet_size);
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_SIMILARITY_BASED_SEQUENCE_WEIGHTING_
