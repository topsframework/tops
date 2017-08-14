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
#include "helper/PairHiddenMarkovModel.hpp"

// Standard headers
#include <cmath>
#include <vector>

// Internal headers
#include "helper/DiscreteIIDModel.hpp"

#include "model/Probability.hpp"

namespace tops {
namespace helper {

using IID = model::DiscreteIIDModel;
using Probability = model::Probability;
using Probabilities = std::vector<Probability>;

/*----------------------------------------------------------------------------*/
/*                                 FUNCTIONS                                  */
/*----------------------------------------------------------------------------*/

model::PairHiddenMarkovModelPtr createGlobalAlignmentPairHMM() {
  /*
   *                  τ
   *  .------------------------------.
   *  |                    ε         |
   *  |                   ...        |
   *  |           δ       \ v     τ  |
   *  |   .-------------->(X)----.   |
   *  |  /   1-2δ-τ      .^/      \  |
   *  | / .---------. δ / / 1-ε-τ  \ |
   *  |/.´         ..`./v´          vv
   * (B)   1-2δ-τ |  `(M)---- τ --->(E)
   *   \           `'^ \^.          ^
   *    \             δ \ \ 1-ε-τ  /
   *     \               'v\      /
   *      '-------------->(Y)----'
   *              δ       / ^     τ
   *                      '''
   *                       ε
   */

  Probability delta = 0.2, epsilon = 0.2, tau = 0.1;
  typename model::PairHiddenMarkovModel::State::Id B = 0, M = 1, X = 2, Y = 3, E = 4;

  std::vector<std::vector<Probabilities>> emissions {
    {
       /*  B  */ /*  0  */ /*  1  */ /* gaṕ */
       /*  0  */{   0.0   ,   0.0   ,   0.0   },
       /*  1  */{   0.0   ,   0.0   ,   0.0   },
       /* gaṕ */{   0.0   ,   0.0   ,   1.0   },
    },
    {
       /*  M  */ /*  0  */ /*  1  */ /* gaṕ */
       /*  0  */{   0.4   ,   0.1   ,   0.0   },
       /*  1  */{   0.1   ,   0.4   ,   0.0   },
       /* gaṕ */{   0.0   ,   0.0   ,   0.0   },
    },
    {
       /*  X  */ /*  0  */ /*  1  */ /* gaṕ */
       /*  0  */{   0.0   ,   0.0   ,   0.0   },
       /*  1  */{   0.0   ,   0.0   ,   0.0   },
       /* gaṕ */{   0.5   ,   0.5   ,   0.0   },
    },
    {
       /*  Y  */ /*  0  */ /*  1  */ /* gaṕ */
       /*  0  */{   0.0   ,   0.0   ,   0.5   },
       /*  1  */{   0.0   ,   0.0   ,   0.5   },
       /* gaṕ */{   0.0   ,   0.0   ,   0.0   },
    },
    {
       /*  E  */ /*  0  */ /*  1  */ /* gaṕ */
       /*  0  */{   0.0   ,   0.0   ,   0.0   },
       /*  1  */{   0.0   ,   0.0   ,   0.0   },
       /* gaṕ */{   0.0   ,   0.0   ,   1.0   },
    },
  };

  std::vector<Probabilities> transitions {
              /* B */ /*     M     */ /*  X  */ /*  Y  */ /* E */
    /* B */ {   0.0  , 1-2*delta-tau ,  delta  ,  delta  ,  tau   },
    /* M */ {   0.0  , 1-2*delta-tau ,  delta  ,  delta  ,  tau   },
    /* X */ {   0.0  , 1-epsilon-tau , epsilon ,   0.0   ,  tau   },
    /* Y */ {   0.0  , 1-epsilon-tau ,   0.0   , epsilon ,  tau   },
    /* E */ {   0.0  ,      0.0      ,   0.0   ,   0.0   ,  1.0   },
  };

  std::vector<model::PairHiddenMarkovModel::StatePtr> states = {
    model::PairHiddenMarkovModel::SilentState::make(
      /* id         */ B,
      /* emission   */ IID::make(emissions[B]),
      /* transition */ IID::make(transitions[B])),

    model::PairHiddenMarkovModel::MatchState::make(
      /* id         */ M,
      /* emission   */ IID::make(emissions[M]),
      /* transition */ IID::make(transitions[M])),

    model::PairHiddenMarkovModel::Gap1State::make(
      /* id         */ X,
      /* emission   */ IID::make(emissions[X]),
      /* transition */ IID::make(transitions[X])),

    model::PairHiddenMarkovModel::Gap2State::make(
      /* id         */ Y,
      /* emission   */ IID::make(emissions[Y]),
      /* transition */ IID::make(transitions[Y])),

    model::PairHiddenMarkovModel::SilentState::make(
      /* id         */ E,
      /* emission   */ IID::make(emissions[E]),
      /* transition */ IID::make(transitions[E]))
  };

  states[B]->addSuccessor(M);
  states[B]->addSuccessor(X);
  states[B]->addSuccessor(Y);
  states[B]->addSuccessor(E);

  states[M]->addPredecessor(B);
  states[M]->addPredecessor(M);
  states[M]->addPredecessor(X);
  states[M]->addPredecessor(Y);
  states[M]->addSuccessor(M);
  states[M]->addSuccessor(X);
  states[M]->addSuccessor(Y);
  states[M]->addSuccessor(E);

  states[X]->addPredecessor(B);
  states[X]->addPredecessor(M);
  states[X]->addPredecessor(X);
  states[X]->addSuccessor(M);
  states[X]->addSuccessor(X);
  states[X]->addSuccessor(E);

  states[Y]->addPredecessor(B);
  states[Y]->addPredecessor(M);
  states[Y]->addPredecessor(Y);
  states[Y]->addSuccessor(M);
  states[Y]->addSuccessor(Y);
  states[Y]->addSuccessor(E);

  states[E]->addPredecessor(B);
  states[E]->addPredecessor(M);
  states[E]->addPredecessor(X);
  states[E]->addPredecessor(Y);

  return std::make_shared<model::PairHiddenMarkovModel>(states, 5, 2);
}

/*----------------------------------------------------------------------------*/

model::PairHiddenMarkovModelPtr createUntrainedGlobalAlignmentPairHMM() {
  /*
   *                  τ
   *  .------------------------------.
   *  |                    ε         |
   *  |                   ...        |
   *  |           δ       \ v     τ  |
   *  |   .-------------->(X)----.   |
   *  |  /   1-2δ-τ      .^/      \  |
   *  | / .---------. δ / / 1-ε-τ  \ |
   *  |/.´         ..`./v´          vv
   * (B)   1-2δ-τ |  `(M)---- τ --->(E)
   *   \           `'^ \^.          ^
   *    \             δ \ \ 1-ε-τ  /
   *     \               'v\      /
   *      '-------------->(Y)----'
   *              δ       / ^     τ
   *                      '''
   *                       ε
   */

  typename model::PairHiddenMarkovModel::State::Id
    B = 0, M = 1, X = 2, Y = 3, E = 4;

  std::vector<std::vector<Probabilities>> emissions {
    {
       /*  B  */ /*  0  */ /*  1  */ /* gap */
       /*  0  */{   0.0   ,   0.0   ,   0.0   },
       /*  1  */{   0.0   ,   0.0   ,   0.0   },
       /* gap */{   0.0   ,   0.0   ,   1.0   },
    },
    {
       /*  M  */ /*  0  */ /*  1  */ /* gap */
       /*  0  */{   0.25  ,   0.25  ,   0.0   },
       /*  1  */{   0.25  ,   0.25  ,   0.0   },
       /* gap */{   0.0   ,   0.0   ,   0.0   },
    },
    {
       /*  X  */ /*  0  */ /*  1  */ /* gap */
       /*  0  */{   0.0   ,   0.0   ,   0.0   },
       /*  1  */{   0.0   ,   0.0   ,   0.0   },
       /* gap */{   0.5   ,   0.5   ,   0.0   },
    },
    {
       /*  Y  */ /*  0  */ /*  1  */ /* gap */
       /*  0  */{   0.0   ,   0.0   ,   0.5   },
       /*  1  */{   0.0   ,   0.0   ,   0.5   },
       /* gap */{   0.0   ,   0.0   ,   0.0   },
    },
    {
       /*  E  */ /*  0  */ /*  1  */ /* gap */
       /*  0  */{   0.0   ,   0.0   ,   0.0   },
       /*  1  */{   0.0   ,   0.0   ,   0.0   },
       /* gap */{   0.0   ,   0.0   ,   1.0   },
    },
  };

  std::vector<Probabilities> transitions {
             /* B */ /* M */ /* X */ /* Y */ /* E */
    /* B */ {  0.0  , 1.0/4 , 1.0/4 , 1.0/4 , 1.0/4 },
    /* M */ {  0.0  , 1.0/4 , 1.0/4 , 1.0/4 , 1.0/4 },
    /* X */ {  0.0  , 1.0/3 , 1.0/3 ,  0.0  , 1.0/3 },
    /* Y */ {  0.0  , 1.0/3 ,  0.0  , 1.0/3 , 1.0/3 },
    /* E */ {  0.0  ,  0.0  ,  0.0  ,  0.0  ,  1.0  },
  };

  std::vector<model::PairHiddenMarkovModel::StatePtr> states = {
    model::PairHiddenMarkovModel::SilentState::make(
      /* id         */ B,
      /* emission   */ IID::make(emissions[B]),
      /* transition */ IID::make(transitions[B])),

    model::PairHiddenMarkovModel::MatchState::make(
      /* id         */ M,
      /* emission   */ IID::make(emissions[M]),
      /* transition */ IID::make(transitions[M])),

    model::PairHiddenMarkovModel::Gap1State::make(
      /* id         */ X,
      /* emission   */ IID::make(emissions[X]),
      /* transition */ IID::make(transitions[X])),

    model::PairHiddenMarkovModel::Gap2State::make(
      /* id         */ Y,
      /* emission   */ IID::make(emissions[Y]),
      /* transition */ IID::make(transitions[Y])),

    model::PairHiddenMarkovModel::SilentState::make(
      /* id         */ E,
      /* emission   */ IID::make(emissions[E]),
      /* transition */ IID::make(transitions[E]))
  };

  states[B]->addSuccessor(M);
  states[B]->addSuccessor(X);
  states[B]->addSuccessor(Y);
  states[B]->addSuccessor(E);

  states[M]->addPredecessor(B);
  states[M]->addPredecessor(M);
  states[M]->addPredecessor(X);
  states[M]->addPredecessor(Y);
  states[M]->addSuccessor(M);
  states[M]->addSuccessor(X);
  states[M]->addSuccessor(Y);
  states[M]->addSuccessor(E);

  states[X]->addPredecessor(B);
  states[X]->addPredecessor(M);
  states[X]->addPredecessor(X);
  states[X]->addSuccessor(M);
  states[X]->addSuccessor(X);
  states[X]->addSuccessor(E);

  states[Y]->addPredecessor(B);
  states[Y]->addPredecessor(M);
  states[Y]->addPredecessor(Y);
  states[Y]->addSuccessor(M);
  states[Y]->addSuccessor(Y);
  states[Y]->addSuccessor(E);

  states[E]->addPredecessor(B);
  states[E]->addPredecessor(M);
  states[E]->addPredecessor(X);
  states[E]->addPredecessor(Y);

  return std::make_shared<model::PairHiddenMarkovModel>(states, 5, 2);
}

/*----------------------------------------------------------------------------*/

}  // namespace helper
}  // namespace tops
