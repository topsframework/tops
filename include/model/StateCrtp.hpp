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

#ifndef TOPS_MODEL_STATE_CRTP_
#define TOPS_MODEL_STATE_CRTP_

// Standard headers
#include <memory>
#include <vector>

// Internal headers
#include "model/State.hpp"
#include "model/SimpleSerializer.hpp"

namespace tops {
namespace model {

// Forward declaration
template<typename EmissionModel, typename TransitionModel, typename Derived>
class StateCrtp;

/**
 * @typedef StateCrtpPtr
 * @brief Alias of pointer to StateCrtp.
 */
template<typename EmissionModel, typename TransitionModel, typename Derived>
using StateCrtpPtr
  = std::shared_ptr<StateCrtp<EmissionModel, TransitionModel, Derived>>;

/**
 * @class StateCrtp
 * @brief TODO
 */
template<typename EmissionModel, typename TransitionModel, typename Derived>
class StateCrtp
    : public std::enable_shared_from_this<
               StateCrtp<EmissionModel, TransitionModel, Derived>>,
      public virtual State<EmissionModel, TransitionModel> {
 public:
  // Alias
  using Base = State<EmissionModel, TransitionModel>;
  using Self = StateCrtp<EmissionModel, TransitionModel, Derived>;
  using SelfPtr = std::shared_ptr<Self>;
  using DerivedPtr = std::shared_ptr<Derived>;

  using Id = typename Base::Id;
  using Position = typename Base::Position;
  using Dimension = typename Base::Dimension;

  using EmissionModelPtr = std::shared_ptr<EmissionModel>;
  using TransitionModelPtr = std::shared_ptr<TransitionModel>;

  using SSPtr = SimpleSerializerPtr<Derived>;

  // Static methods
  template<typename... Args>
  static DerivedPtr make(Args&&... args);

  // Virtual methods
  SerializerPtr serializer(TranslatorPtr translator) override;

  Id id() const override;

  EmissionModelPtr emission() override;
  void emission(EmissionModelPtr ptr) override;

  TransitionModelPtr transition() override;
  void transition(TransitionModelPtr ptr) override;

  DurationPtr duration() override;
  void duration(DurationPtr ptr) override;

  void addPredecessor(Id id) override;
  std::vector<Id>& predecessors() override;
  const std::vector<Id>& predecessors() const override;

  void addSuccessor(Id id) override;
  std::vector<Id>& successors() override;
  const std::vector<Id>& successors() const override;

  size_t beginExtension() const override;
  size_t endExtension() const override;

  size_t beginPhase() const override;
  size_t endPhase() const override;

  // Virtual methods
  virtual void serialize(SSPtr serializer);

 protected:
  // Constructors
  StateCrtp(Id id, EmissionModelPtr emission,
                   TransitionModelPtr transition,
                   DurationPtr duration,
                   std::pair<size_t, size_t> extensions,
                   std::pair<size_t, size_t> phases);

  // Instance variables
  Id _id;

  EmissionModelPtr _emission;
  TransitionModelPtr _transition;
  DurationPtr _duration;

  std::pair<size_t, size_t> _extensions;
  std::pair<size_t, size_t> _phases;

  std::vector<Id> _predecessors;
  std::vector<Id> _successors;

 private:
  // Concrete methods
  DerivedPtr make_shared();
};

}  // namespace model
}  // namespace tops

// Implementation header
#include "model/StateCrtp.ipp"

#endif  // TOPS_MODEL_STATE_CRTP_
