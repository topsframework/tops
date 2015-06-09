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

#ifndef TOPS_MODEL_PROBABILISTIC_MODEL_CRTP_
#define TOPS_MODEL_PROBABILISTIC_MODEL_CRTP_

// Standard headers
#include <memory>

// ToPS headers
#include "model/Sequence.hpp"
#include "model/ProbabilisticModel.hpp"

// ToPS templates
#include "model/Generator.tcc"
#include "model/SimpleGenerator.tcc"

namespace tops {
namespace model {

// Forward declaration
template<typename Derived>
class ProbabilisticModelCrtp;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                      CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/**
 * @typedef ProbabilisticModelCrtpPtr
 * @brief Alias of pointer to ProbabilisticModelCrtp.
 */
template<typename Derived>
using ProbabilisticModelPtrCrtp
  = std::shared_ptr<ProbabilisticModelCrtp<Derived>>;

/**
 * @class ProbabilisticModelCrtp
 * @brief Implementation of front-end with CRTP to inject methods in  subclasses
 */
template<typename Derived>
class ProbabilisticModelCrtp : public ProbabilisticModel {
 public:
  // Alias
  using Base = ProbabilisticModel;
  using DerivedPtr = std::shared_ptr<Derived>;

  template<template<typename Target> class Decorator>
  using SGPtr = SimpleGeneratorPtr<Decorator, Derived>;

  // Overriden methods
  GeneratorPtr<Standard> sequenceGenerator() override;

  // Virtual methods
  virtual Standard<Sequence>
  simpleChooseSequence(SGPtr<Standard> generator,
                       unsigned int size,
                       unsigned int phase = 0) const;
};

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                 IMPLEMENTATION
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename Derived>
GeneratorPtr<Standard> ProbabilisticModelCrtp<Derived>::sequenceGenerator() {
  return SimpleGenerator<Standard, Derived>::make(
    std::static_pointer_cast<Derived>(
      static_cast<Derived *>(this)->shared_from_this()));
}

/*----------------------------------------------------------------------------*/
/*                              VIRTUAL METHODS                               */
/*----------------------------------------------------------------------------*/

template<typename Derived>
Standard<Sequence> ProbabilisticModelCrtp<Derived>::simpleChooseSequence(
    SGPtr<Standard> generator,
    unsigned int size,
    unsigned int phase) const {
  Sequence s;
  for (unsigned int k = 0; k < size; k++)
      s.push_back(choose(s, k));
  return s;
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_PROBABILISTIC_MODEL_
