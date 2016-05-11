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
#include <memory>
#include <vector>
#include <map>

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

  // Alias
  using Base = ProbabilisticModelCrtp<SimilarityBasedSequenceWeighting>;

  using Self = SimilarityBasedSequenceWeighting;
  using SelfPtr = SimilarityBasedSequenceWeightingPtr;

  // Constructors
  SimilarityBasedSequenceWeighting(std::map<Sequence, double> counter,
                                   double normalizer,
                                   int skip_offset,
                                   int skip_length,
                                   Sequence skip_sequence);

  // Static methods
  static SimilarityBasedSequenceWeightingPtr make(
      std::map<Sequence, double> counter,
      double normalizer,
      int skip_offset,
      int skip_length,
      Sequence skip_sequence);

  static SelfPtr train(TrainerPtr<Standard, Self> trainer,
                       standard_training_algorithm,
                       unsigned int alphabet_size,
                       int skip_offset,
                       int skip_length,
                       Sequence skip_sequence);

  // Overriden methods
  void initializeCache(CEPtr<Standard> evaluator,
                       unsigned int phase) override;

  Probability evaluateSymbol(SEPtr<Standard> evaluator,
                             unsigned int pos,
                             unsigned int phase) const override;
  Probability evaluateSequence(SEPtr<Standard> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  Probability evaluateSequence(CEPtr<Standard> evaluator,
                               unsigned int begin,
                               unsigned int end,
                               unsigned int phase) const override;

  Standard<Symbol> drawSymbol(SGPtr<Standard> generator,
                              unsigned int pos,
                              unsigned int phase,
                              const Sequence& context) const override;

 private:
  // Instance variables
  Cache _scores;
  std::map<Sequence, double> _counter;
  unsigned int _skip_offset;
  unsigned int _skip_length;
  Sequence _skip_sequence;
  double _normalizer;

  // Static methods
  static double calculate_normalizer(int skip_length,
                                     int skip_offset,
                                     int max_length,
                                     std::map<Sequence, double>& counter,
                                     int alphabet_size);
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_SIMILARITY_BASED_SEQUENCE_WEIGHTING_
