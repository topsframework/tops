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

#ifndef TOPS_MODEL_FIXED_SEQUENCE_AT_POSITION_
#define TOPS_MODEL_FIXED_SEQUENCE_AT_POSITION_

// Standard headers
#include <memory>
#include <vector>
#include <utility>

// Internal headers
#include "model/DiscreteIIDModel.hpp"
#include "model/ProbabilisticModelCrtp.hpp"

namespace tops {
namespace model {

// Forward declarations
class FixedSequenceAtPosition;

/**
 * @typedef FixedSequenceAtPositionPtr
 * @brief Alias of pointer to FixedSequenceAtPosition.
 */
using FixedSequenceAtPositionPtr
  = std::shared_ptr<FixedSequenceAtPosition>;

/**
 * @class FixedSequenceAtPosition
 * @brief TODO
 */
class FixedSequenceAtPosition
    : public ProbabilisticModelCrtp<FixedSequenceAtPosition> {
 public:
  // Aliases
  using Self = FixedSequenceAtPosition;
  using SelfPtr = FixedSequenceAtPositionPtr;
  using Base = ProbabilisticModelCrtp<Self>;

  /*=============================[ CONSTRUCTORS ]=============================*/

  FixedSequenceAtPosition(ProbabilisticModelPtr model,
                          int position,
                          Sequence sequence,
                          DiscreteIIDModelPtr distr);

  /*==========================[ OVERRIDEN METHODS ]===========================*/
  /*-------------------------( Probabilistic Model )--------------------------*/

  // StandardEvaluator
  Probability evaluateSymbol(SEPtr<Standard> evaluator,
                             size_t pos,
                             size_t phase) const override;
  Probability evaluateSequence(SEPtr<Standard> evaluator,
                               size_t begin,
                               size_t end,
                               size_t phase) const override;

  // CachedEvaluator
  void initializeCache(CEPtr<Standard> evaluator,
                       size_t phase) override;
  Probability evaluateSymbol(CEPtr<Standard> evaluator,
                             size_t pos,
                             size_t phase) const override;
  Probability evaluateSequence(CEPtr<Standard> evaluator,
                               size_t begin,
                               size_t end,
                               size_t phase) const override;

  // StandardGenerator
  Standard<Symbol> drawSymbol(SGPtr<Standard> generator,
                              size_t pos,
                              size_t phase,
                              const Sequence& context) const override;
  Standard<Sequence> drawSequence(SGPtr<Standard> generator,
                                  size_t size,
                                  size_t phase) const override;

  // SimpleSerializer
  void serialize(SSPtr serializer) override;

 private:
  // Instance variables
  ProbabilisticModelPtr _model;
  int _position;
  Sequence _sequence;
  DiscreteIIDModelPtr _probabilities;

  /*==========================[ CONCRETE METHODS ]============================*/

  void addSequence(Sequence& h) const;
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_FIXED_SEQUENCE_AT_POSITION_
