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

// Interface header
#include "helper/ContextSensitiveHiddenMarkovModel.hpp"

// Standard headers
#include <cmath>
#include <memory>
#include <vector>

// Internal headers
#include "helper/DiscreteIIDModel.hpp"

#include "model/Probability.hpp"

namespace tops {
namespace helper {

using IID = model::DiscreteIIDModel;
using CSHMM = model::ContextSensitiveHiddenMarkovModel;

using Probability = model::Probability;
using Probabilities = std::vector<Probability>;

/*----------------------------------------------------------------------------*/
/*                                 FUNCTIONS                                  */
/*----------------------------------------------------------------------------*/

std::shared_ptr<CSHMM> createDishonestCoinCasinoCSHMM() {
  /*
   *                τ
   *  .---------------------------.
   *  |           1-σ-τ           |
   *  |            ...            |
   *  |   (1-τ)/2  \ v     τ      |
   *  | .--------->(H)----------. |
   *  |/           | ^           vv
   * (B)         σ | | σ         (E)
   *   \           v |           ^
   *    `--------->(D)----------'
   *      (1-τ)/2  / ^     τ
   *               '''
   *              1-σ-τ
   */

  Probability sigma = 0.7, tau = 0.1;
  typename CSHMM::State::Id B = 0, H = 1, D = 2, E = 3;

  std::vector<Probabilities> emissions {
    {
      /*  B  */ /*  0  */ /*  1  */ /* gap */
               {   0.0   ,   0.0   ,   1.0   },
    },
    {
      /*  H  */ /*  0  */ /*  1  */ /* gap */
               {   0.5   ,   0.5   ,   0.0   },
    },
    {
      /*  D  */ /*  0  */ /*  1  */ /* gap */
               {   0.2   ,   0.8   ,   0.0   },
    },
    {
      /*  E  */ /*  0  */ /*  1  */ /* gap */
               {   0.0   ,   0.0   ,   1.0   },
    },
  };

  std::vector<Probabilities> transitions {
              /* B */ /*    H    */ /*    D    */ /* E */
    /* B */ {   0.0  ,  (1-tau)/2  ,  (1-tau)/2  ,  tau   },
    /* H */ {   0.0  ,    sigma    , 1-sigma-tau ,  tau   },
    /* D */ {   0.0  , 1-sigma-tau ,    sigma    ,  tau   },
    /* E */ {   0.0  ,     0.0     ,     0.0     ,  1.0   },
  };

  std::vector<CSHMM::StatePtr> states = {
    CSHMM::SilentState::make(
      /* id         */ B,
      /* emission   */ IID::make(emissions[B]),
      /* transition */ IID::make(transitions[B])),

    CSHMM::MatchState::make(
      /* id         */ H,
      /* emission   */ IID::make(emissions[H]),
      /* transition */ IID::make(transitions[H])),

    CSHMM::MatchState::make(
      /* id         */ D,
      /* emission   */ IID::make(emissions[D]),
      /* transition */ IID::make(transitions[D])),

    CSHMM::SilentState::make(
      /* id         */ E,
      /* emission   */ IID::make(emissions[E]),
      /* transition */ IID::make(transitions[E]))
  };

  states[B]->addSuccessor(H);
  states[B]->addSuccessor(D);
  states[B]->addSuccessor(E);

  states[H]->addPredecessor(B);
  states[H]->addPredecessor(H);
  states[H]->addPredecessor(D);
  states[H]->addSuccessor(H);
  states[H]->addSuccessor(D);
  states[H]->addSuccessor(E);

  states[D]->addPredecessor(B);
  states[D]->addPredecessor(H);
  states[D]->addPredecessor(D);
  states[D]->addSuccessor(H);
  states[D]->addSuccessor(D);
  states[D]->addSuccessor(E);

  states[E]->addPredecessor(B);
  states[E]->addPredecessor(H);
  states[E]->addPredecessor(D);

  return std::make_shared<CSHMM>(states, 4, 2);
}

/*----------------------------------------------------------------------------*/

std::shared_ptr<CSHMM> createUntrainedDishonestCoinCasinoCSHMM() {
  /*
   *                τ
   *  .---------------------------.
   *  |           1-σ-τ           |
   *  |            ...            |
   *  |   (1-τ)/2  \ v     τ      |
   *  | .--------->(H)----------. |
   *  |/           | ^           vv
   * (B)         σ | | σ         (E)
   *   \           v |           ^
   *    `--------->(D)----------'
   *      (1-τ)/2  / ^     τ
   *               '''
   *              1-σ-τ
   */

  typename CSHMM::State::Id B = 0, H = 1, D = 2, E = 3;

  std::vector<Probabilities> emissions {
    {
      /*  B  */ /*  0  */ /*  1  */ /* gap */
               {   0.0   ,   0.0   ,   1.0   },
    },
    {
      /*  H  */ /*  0  */ /*  1  */ /* gap */
               {   0.5   ,   0.5   ,   0.0   },
    },
    {
      /*  D  */ /*  0  */ /*  1  */ /* gap */
               {   0.5   ,   0.5   ,   0.0   },
    },
    {
      /*  E  */ /*  0  */ /*  1  */ /* gap */
               {   0.0   ,   0.0   ,   1.0   },
    },
  };

  std::vector<Probabilities> transitions {
             /* B */ /* H */ /* D */ /* E */
    /* B */ {  0.0  , 1.0/3 , 1.0/3 , 1.0/3 },
    /* H */ {  0.0  , 1.0/3 , 1.0/3 , 1.0/3 },
    /* D */ {  0.0  , 1.0/3 , 1.0/3 , 1.0/3 },
    /* E */ {  0.0  ,  0.0  ,  0.0  ,  1.0  },
  };

  std::vector<CSHMM::StatePtr> states = {
    CSHMM::SilentState::make(
      /* id         */ B,
      /* emission   */ IID::make(emissions[B]),
      /* transition */ IID::make(transitions[B])),

    CSHMM::MatchState::make(
      /* id         */ H,
      /* emission   */ IID::make(emissions[H]),
      /* transition */ IID::make(transitions[H])),

    CSHMM::MatchState::make(
      /* id         */ D,
      /* emission   */ IID::make(emissions[D]),
      /* transition */ IID::make(transitions[D])),

    CSHMM::SilentState::make(
      /* id         */ E,
      /* emission   */ IID::make(emissions[E]),
      /* transition */ IID::make(transitions[E]))
  };

  states[B]->addSuccessor(H);
  states[B]->addSuccessor(D);
  states[B]->addSuccessor(E);

  states[H]->addPredecessor(B);
  states[H]->addPredecessor(H);
  states[H]->addPredecessor(D);
  states[H]->addSuccessor(H);
  states[H]->addSuccessor(D);
  states[H]->addSuccessor(E);

  states[D]->addPredecessor(B);
  states[D]->addPredecessor(H);
  states[D]->addPredecessor(D);
  states[D]->addSuccessor(H);
  states[D]->addSuccessor(D);
  states[D]->addSuccessor(E);

  states[E]->addPredecessor(B);
  states[E]->addPredecessor(H);
  states[E]->addPredecessor(D);

  return std::make_shared<CSHMM>(states, 4, 2);
}

/*----------------------------------------------------------------------------*/

}  // namespace helper
}  // namespace tops
