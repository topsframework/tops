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
/*                           LATE EVALUATED BOOLEANS                          */
/*============================================================================*/

template<typename T>
struct always_true : public std::integral_constant<bool, true> {};

template<typename T>
struct always_false : public std::integral_constant<bool, false> {};

/*============================================================================*/
/*                              NON CONST RETURN                              */
/*============================================================================*/

template <typename T> struct non_const_return { typedef T type; };

template <typename T> struct non_const_return<T const  > { typedef T   type; };
template <typename T> struct non_const_return<T const& > { typedef T&  type; };
template <typename T> struct non_const_return<T const* > { typedef T*  type; };
template <typename T> struct non_const_return<T const&&> { typedef T&& type; };

template <typename T>
using non_const_return_t = typename non_const_return<T>::type;

template<typename T>
non_const_return_t<T> cast_to_non_const(T t) {
    return (non_const_return_t<T>) t;
}

/*============================================================================*/
/*                              CLASS OF POINTER                              */
/*============================================================================*/

template<typename T> struct class_of;

template<typename T> struct class_of<T*> {
  using type = typename std::remove_cv<
    typename std::remove_pointer<T*>::type>::type;
};

template<typename T>
using class_of_t = typename class_of<T>::type;

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

#define GENERATE_METHOD_DELEGATOR(method, delegatedObject)                     \
                                                                               \
GENERATE_HAS_MEMBER_METHOD(method);                                            \
                                                                               \
template<typename... Args>                                                     \
inline auto method##Impl(Args... args) const                                   \
    -> decltype(cast_to_non_const(this)->method(args...)) {                    \
                                                                               \
  using MethodType = typename inject_first_parameter<                          \
    std::shared_ptr<class_of_t<decltype(this)>>,                               \
    decltype(&class_of_t<decltype(this)>::method)                              \
  >::type;                                                                     \
                                                                               \
  using DelegatedType = typename std::remove_cv<                               \
    typename std::remove_pointer<decltype(this->delegatedObject)>::type        \
  >::type::element_type;                                                       \
                                                                               \
  return method##Impl(                                                         \
    typename has_method_##method<DelegatedType, MethodType>::tag(),            \
    std::forward<Args>(args)...);                                              \
}                                                                              \
                                                                               \
template<typename... Args>                                                     \
inline auto method##Impl(Args... args)                                         \
    -> decltype(this->method(args...)) {                                       \
  return (non_const_return_t<decltype(this->method(args...))>) (               \
    static_cast<const class_of_t<decltype(this)> *>(this)->method##Impl(       \
      std::forward<Args>(args)...));                                           \
}                                                                              \
                                                                               \
template<typename... Args>                                                     \
inline auto method##Impl(no_##method##_tag, Args... args) const                \
    -> decltype(cast_to_non_const(this)->method(args...)) {                    \
  static_assert(always_false<decltype(this)>::value,                           \
                "Missing implementation of member function '" #method "'!");   \
  throw std::logic_error("Calling from base class with no 'method'");          \
}                                                                              \
                                                                               \
template<typename... Args>                                                     \
inline auto method##Impl(no_##method##_tag, Args... args)                      \
    -> decltype(this->method(args...)) {                                       \
  return (non_const_return_t<decltype(this->method(args...))>) (               \
    static_cast<const class_of_t<decltype(this)>*>(this)->method##Impl(        \
      no_##method##_tag(), std::forward<Args>(args)...));                      \
}                                                                              \
                                                                               \
template<typename... Args>                                                     \
inline auto method##Impl(has_##method##_tag, Args... args) const               \
    -> decltype(cast_to_non_const(this)->method(args...)) {                    \
  return (this->delegatedObject)->method(                                      \
    std::static_pointer_cast<class_of_t<decltype(this)>>(                      \
      const_cast<class_of_t<decltype(this)>*>(this)->shared_from_this()),      \
    std::forward<Args>(args)...);                                              \
}                                                                              \
                                                                               \
template<typename... Args>                                                     \
inline auto method##Impl(has_##method##_tag, Args... args)                     \
    -> decltype(this->method(args...)) {                                       \
  return (non_const_return_t<decltype(this->method(args...))>) (               \
    static_cast<const class_of_t<decltype(this)>*>(this)->method##Impl(        \
      has_##method##_tag(), std::forward<Args>(args)...));                     \
}

/*============================================================================*/
/*                           MEMBER DELEGATOR CALL                            */
/*============================================================================*/

#define CALL_METHOD_DELEGATOR(method, ...)                                     \
do { return method##Impl(__VA_ARGS__); } while (false)

}  // namespace model
}  // namespace tops

#endif
