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

#ifndef TOPS_MODEL_MEMBER_DELEGATOR_
#define TOPS_MODEL_MEMBER_DELEGATOR_

// ToPS templates
#include "model/MemberDetector.tcc"

namespace tops {
namespace model {

/*============================================================================*/
/*                          FIRST PARAMETER INJECTION                         */
/*============================================================================*/

template<typename Ptr, typename Dummy> struct inject_first_parameter;

template<typename Ptr, typename Klass, typename Result, typename... Args>
struct inject_first_parameter<Ptr, Result(Klass::*)(Args...)> {
  using type = Result(Ptr, Args...);
};

template<typename Ptr, typename Klass, typename Result, typename... Args>
struct inject_first_parameter<Ptr, Result(Klass::*)(Args...) const> {
  using type = const Result(Ptr, Args...);
};

/*============================================================================*/
/*                         MEMBER DELEGATOR GENERATION                        */
/*============================================================================*/

#define GENERATE_METHOD_DELEGATOR(interface, implementation, delegatedObject)  \
                                                                               \
GENERATE_HAS_MEMBER_METHOD(implementation);                                    \
                                                                               \
template<typename... Args>                                                     \
inline auto interface##Impl(Args... args) const                                \
    -> decltype(this->interface(args...)) {                                    \
                                                                               \
  using Klass = typename std::remove_cv<                                       \
    typename std::remove_pointer<decltype(this)>::type>::type;                 \
  using MethodType = typename inject_first_parameter<                          \
    std::shared_ptr<Klass>, decltype(&Klass::interface)>::type;                \
                                                                               \
  using DelegatedType = typename std::remove_cv<                               \
    typename std::remove_pointer<decltype(delegatedObject)>::type>::type;      \
                                                                               \
  return interface##Impl(                                                      \
    typename has_method_##implementation<                                      \
      typename DelegatedType::element_type, MethodType>::tag(),                \
    std::forward<Args>(args)...);                                              \
}                                                                              \
                                                                               \
template<typename... Args>                                                     \
inline auto interface##Impl(no_##implementation##_tag, Args... args) const     \
    -> decltype(this->interface(args...)) {                                    \
  static_assert(is_base, "Class don't have method 'implementation'!");         \
  throw std::logic_error("Calling from base class with no 'implementation'");  \
}                                                                              \
                                                                               \
template<typename... Args>                                                     \
inline auto interface##Impl(has_##implementation##_tag, Args... args) const    \
    -> decltype(this->interface(args...)) {                                    \
                                                                               \
  using Klass = typename std::remove_cv<                                       \
    typename std::remove_pointer<decltype(this)>::type>::type;                 \
                                                                               \
  return (delegatedObject)->implementation(                                    \
    std::static_pointer_cast<Klass>(                                           \
      const_cast<Klass*>(this)->shared_from_this()),                           \
    std::forward<Args>(args)...);                                              \
}

/*============================================================================*/
/*                           MEMBER DELEGATOR CALL                            */
/*============================================================================*/

#define CALL_METHOD_DELEGATOR(interface, implementation, delegatedObject, ...) \
do { return interface##Impl(__VA_ARGS__); } while (false)

}  // namespace model
}  // namespace tops

#endif
