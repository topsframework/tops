/***********************************************************************/
/*  Copyright 2016 ToPS                                                */
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
#include <string>
#include <vector>
#include <utility>

// Internal headers
#include "config/ConfigVisitor.hpp"

namespace tops {
namespace config {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
BasicConfig<ID, Base, Options...>::BasicConfig(const std::string &path,
                                               const std::string &label)
    : Base(path, label) {
}

/*----------------------------------------------------------------------------*/
/*                              STATIC VARIABLES                              */
/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
template<typename... Params>
std::shared_ptr<BasicConfig<ID, Base, Options...>>
BasicConfig<ID, Base, Options...>::make(Params&&... params) {
  return std::make_shared<Self>(std::forward<Params>(params)...);
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
void BasicConfig<ID, Base, Options...>::accept(ConfigVisitor &visitor) const {
  auto ptr = std::static_pointer_cast<Self>(
    const_cast<Self*>(this)->shared_from_this());
  visitor.visit(ptr);
}

/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
void BasicConfig<ID, Base, Options...>::accept(ConfigVisitor &&visitor) const {
  auto ptr = std::static_pointer_cast<Self>(
    const_cast<Self*>(this)->shared_from_this());
  visitor.visit(ptr);
}

/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
std::size_t BasicConfig<ID, Base, Options...>::number_of_options() const {
  return this->Base::number_of_options()
    + std::tuple_size<decltype(
        named_types::forward_as_reference_tuple(attrs_))>::value;
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
template<typename Func>
void BasicConfig<ID, Base, Options...>::for_each(Func&& func) const {
  this->Base::for_each(func);
  named_types::for_each(func, attrs_);
}

/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
template<typename... Args>
void BasicConfig<ID, Base, Options...>::initialize(Args&&... args) {
  static_assert(sizeof...(Args) >= sizeof...(Options),
    "Must have least as many arguments as options in `initialize`");

  initialize_base(std::forward<Args>(args)...);
  initialize_tuple(std::forward<Args>(args)...);
}

/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
template<class Tag>
inline constexpr decltype(auto) BasicConfig<ID, Base, Options...>::get(
    std::enable_if_t<!has_tag<Tag, Options...>()>*) const & {
  return this->Base::template get<Tag>();
}

/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
template<class Tag>
inline constexpr decltype(auto) BasicConfig<ID, Base, Options...>::get(
    std::enable_if_t<has_tag<Tag, Options...>()>*) const & {
  return std::get<Tag>(attrs_);
}

/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
template<class Tag>
inline decltype(auto) BasicConfig<ID, Base, Options...>::get(
    std::enable_if_t<!has_tag<Tag, Options...>()>*) & {
  return this->Base::template get<Tag>();
}

/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
template<class Tag>
inline decltype(auto) BasicConfig<ID, Base, Options...>::get(
    std::enable_if_t<has_tag<Tag, Options...>()>*) & {
  return std::get<Tag>(attrs_);
}

/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
template<class Tag>
inline decltype(auto) BasicConfig<ID, Base, Options...>::get(
    std::enable_if_t<!has_tag<Tag, Options...>()>*) && {
  return this->Base::template get<Tag>();
}

/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
template<class Tag>
inline decltype(auto) BasicConfig<ID, Base, Options...>::get(
    std::enable_if_t<has_tag<Tag, Options...>()>*) && {
  return std::get<Tag>(std::move(attrs_));
}

/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
std::vector<std::shared_ptr<BasicConfig<ID, Base, Options...>>>&
BasicConfig<ID, Base, Options...>::children() {
  return children_;
}

/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
const std::vector<std::shared_ptr<BasicConfig<ID, Base, Options...>>>&
BasicConfig<ID, Base, Options...>::children() const {
  return children_;
}

/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
template<typename... Args>
void BasicConfig<ID, Base, Options...>::initialize_base(Args&&... args) {
  forward_subpack(
    [this](auto&&... types) {
      this->Base::initialize(std::forward<decltype(types)>(types)...); },
    index_range<0, sizeof...(Args) - sizeof...(Options)>(),
    std::forward<Args>(args)...);
}

/*----------------------------------------------------------------------------*/

template<typename ID, typename Base, typename... Options>
template<typename... Args>
void BasicConfig<ID, Base, Options...>::initialize_tuple(Args&&... args) {
  forward_subpack(
    [this](auto&&... types) {
      attrs_ = Tuple(std::forward<decltype(types)>(types)...); },
    index_range<sizeof...(Args) - sizeof...(Options), sizeof...(Args)>(),
    std::forward<Args>(args)...);
}

/*----------------------------------------------------------------------------*/

}  // namespace config
}  // namespace tops
