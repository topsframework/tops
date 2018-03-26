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

// Standard headers
#include <memory>
#include <vector>
#include <utility>

namespace tops {
namespace model {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
StateCrtp<E, T, D>::StateCrtp(
    Id id,
    EmissionModelPtr emission,
    TransitionModelPtr transition,
    DurationPtr duration,
    std::pair<std::size_t, std::size_t> extensions,
    std::pair<std::size_t, std::size_t> phases)
    : _id(std::move(id)),
      _emission(std::move(emission)),
      _transition(std::move(transition)),
      _duration(std::move(duration)),
      _extensions(std::move(extensions)),
      _phases(std::move(phases))
  {
}

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
template<typename... Args>
auto StateCrtp<E, T, D>::make(Args&&... args) -> DerivedPtr {
  return std::make_shared<D>(std::forward<Args>(args)...);
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

/*===============================  SERIALIZER  ===============================*/

template<typename E, typename T, typename D>
SerializerPtr StateCrtp<E, T, D>::serializer(TranslatorPtr translator) {
  return SimpleSerializer<D>::make(make_shared(), translator);
}

/*=================================  OTHERS  =================================*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::id() const -> Id {
  return _id;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::emission() -> EmissionModelPtr {
  return _emission;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
void StateCrtp<E, T, D>::emission(EmissionModelPtr ptr) {
  _emission = ptr;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::transition() -> TransitionModelPtr {
  return _transition;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
void StateCrtp<E, T, D>::transition(TransitionModelPtr ptr) {
  _transition = ptr;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::duration() -> DurationPtr {
  return _duration;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
void StateCrtp<E, T, D>::duration(DurationPtr ptr) {
  _duration = ptr;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
void StateCrtp<E, T, D>::addPredecessor(Id id) {
  _predecessors.push_back(id);
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::predecessors() -> std::vector<Id>& {
  return _predecessors;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::predecessors() const -> const std::vector<Id>& {
  return _predecessors;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
void StateCrtp<E, T, D>::addSuccessor(Id id) {
  _successors.push_back(id);
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::successors() -> std::vector<Id>& {
  return _successors;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::successors() const -> const std::vector<Id>& {
  return _successors;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
std::size_t StateCrtp<E, T, D>::beginExtension() const {
  return _extensions.first;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
std::size_t StateCrtp<E, T, D>::endExtension() const {
  return _extensions.second;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
std::size_t StateCrtp<E, T, D>::beginPhase() const {
  return _phases.first;
}

/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
std::size_t StateCrtp<E, T, D>::endPhase() const {
  return _phases.second;
}

/*----------------------------------------------------------------------------*/
/*                              VIRTUAL METHODS                               */
/*----------------------------------------------------------------------------*/

/*===============================  SERIALIZER  ===============================*/

template<typename E, typename T, typename D>
void StateCrtp<E, T, D>::serialize(SSPtr serializer) {
  serializer->translator()->translate(this->make_shared());
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename E, typename T, typename D>
auto StateCrtp<E, T, D>::make_shared() -> DerivedPtr {
  return std::static_pointer_cast<D>(
    static_cast<D *>(this)->shared_from_this());
}

/*----------------------------------------------------------------------------*/

}  // namespace model
}  // namespace tops
