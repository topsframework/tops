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

// ToPS headers
#include "model/ProbabilisticModel.hpp"

namespace tops {
namespace model {

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
class SimilarityBasedSequenceWeighting : public ProbabilisticModel {
 public:
  // Static methods
  static SimilarityBasedSequenceWeightingPtr make(
      int alphabet_size,
      std::map<Sequence, double> counter,
      double normalizer,
      int skip_offset,
      int skip_length,
      Sequence skip_sequence);
  static SimilarityBasedSequenceWeightingPtr train(
      std::vector<Sequence> training_set,
      unsigned int alphabet_size,
      int skip_offset,
      int skip_length,
      Sequence skip_sequence);

  // Virtual methods
  virtual double evaluatePosition(const Sequence &s, unsigned int i) const;
  virtual double evaluateSequence(const Sequence &s,
                                  unsigned int begin,
                                  unsigned int end) const;
  virtual Symbol choosePosition(const Sequence &s, unsigned int i) const;
  virtual double evaluateWithPrefixSumArray(unsigned int begin, unsigned int end);
  virtual void initializePrefixSumArray(const Sequence &s);

 private:
  // Instance variables
  std::vector<double> _scores;
  std::map<Sequence, double> _counter;
  unsigned int _skip_offset;
  unsigned int _skip_length;
  Sequence _skip_sequence;
  double _normalizer;

  // Constructors
  SimilarityBasedSequenceWeighting(int alphabet_size, std::map<Sequence, double> counter, double normalizer, int skip_offset, int skip_length, Sequence skip_sequence);

  // Static methods
  static double calculate_normalizer(int skip_length, int skip_offset, int max_length, std::map<Sequence, double> & counter, int alphabet_size);
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_SIMILARITY_BASED_SEQUENCE_WEIGHTING_
