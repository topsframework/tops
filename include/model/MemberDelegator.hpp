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

// Standard headers
#include <utility>

// Internal headers
#include "model/MemberDetector.hpp"

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
non_const_return_t<T> non_const_cast(T t) {
  return static_cast<non_const_return_t<T>>(t);
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
/*                              CONTAINER UNPACK                              */
/*============================================================================*/

template<typename Func, template<typename...> class Pack,
         typename Ptr, typename... Args, size_t... I>
auto call_helper(const Func& func, const Ptr& ptr, const Pack<Args...>& params,
                   std::index_sequence<I...>) {
  return func(ptr, std::get<I>(params)...);
}

template<typename Func, template<typename...> class Pack,
         typename Ptr, typename... Args>
auto call(const Func& func, const Ptr& ptr, const Pack<Args...>& params) {
  return call_helper(func, ptr, params, std::index_sequence_for<Args...>{});
}

/*============================================================================*/
/*                          MAKE FROM TUPLE ELEMENTS                          */
/*============================================================================*/

template<template<typename... Ts> class Target, typename Tuple>
struct make_from_tuple_elements;

template<template<typename... Ts> class Target,
         template<typename... Ts> class Tuple, typename... Args>
struct make_from_tuple_elements<Target, Tuple<Args...>> {
  using type = Target<Args...>;
};

/*============================================================================*/
/*                    MEMBER FUNCTION DELEGATOR GENERATION                    */
/*============================================================================*/

#define GENERATE_MEMBER_FUNCTION_DELEGATOR(method, delegatedObject)            \
                                                                               \
template<typename... Args>                                                     \
inline auto method##Impl(Args&&... args) const                                 \
    -> decltype(non_const_cast(this)->method(std::forward<Args>(args)...)) {   \
  return (this->delegatedObject)->method(                                      \
    std::static_pointer_cast<class_of_t<decltype(this)>>(                      \
      const_cast<class_of_t<decltype(this)>*>(this)->shared_from_this()),      \
    std::forward<Args>(args)...);                                              \
}                                                                              \
                                                                               \
template<typename... Args>                                                     \
inline auto method##Impl(Args&&... args)                                       \
    -> decltype(this->method(std::forward<Args>(args)...)) {                   \
  return static_cast<non_const_return_t<decltype(this->method(args...))>>(     \
    static_cast<const class_of_t<decltype(this)>*>(this)->method##Impl(        \
      std::forward<Args>(args)...));                                           \
}

/*============================================================================*/
/*                       MEMBER FUNCTION DELEGATOR CALL                       */
/*============================================================================*/

#define CALL_MEMBER_FUNCTION_DELEGATOR(method, ...)                            \
do { return method##Impl(__VA_ARGS__); } while (false)

/*============================================================================*/
/*                STATIC MEMBER FUNCTION DELEGATOR GENERATION                 */
/*============================================================================*/

#define GENERATE_STATIC_MEMBER_FUNCTION_DELEGATOR(method, delegatedClass)      \
                                                                               \
GENERATE_HAS_STATIC_MEMBER_FUNCTION(delegate);                                 \
GENERATE_HAS_STATIC_MEMBER_FUNCTION(method##Alt);                              \
                                                                               \
template<typename... Args>                                                     \
inline auto method##Impl(Args&&... args) const                                 \
    -> decltype(non_const_cast(this)->method(std::forward<Args>(args)...)) {   \
  if (delegate()) {                                                            \
    return delegatedClass::method(                                             \
      std::static_pointer_cast<class_of_t<decltype(this)>>(                    \
        const_cast<class_of_t<decltype(this)>*>(this)->shared_from_this()),    \
      std::forward<Args>(args)...);                                            \
  }                                                                            \
  return method##Alt();                                                        \
}                                                                              \
                                                                               \
template<typename... Args>                                                     \
inline auto method##Impl(Args&&... args)                                       \
    -> decltype(this->method(std::forward<Args>(args)...)) {                   \
  return static_cast<non_const_return_t<                                       \
            decltype(this->method(std::forward<Args>(args)...))>>(             \
    static_cast<const class_of_t<decltype(this)>*>(this)->method##Impl(        \
      std::forward<Args>(args)...));                                           \
}                                                                              \
                                                                               \
inline auto method##Impl() const                                               \
    -> decltype(non_const_cast(this)->method()) {                              \
  return method##Alt();                                                        \
}                                                                              \
                                                                               \
inline auto method##Impl()                                                     \
    -> decltype(this->method()) {                                              \
  return static_cast<non_const_return_t<decltype(this->method())>>(            \
    static_cast<const class_of_t<decltype(this)>*>(this)->method##Impl());     \
}                                                                              \
                                                                               \
template<typename _T = void>                                                   \
inline constexpr auto method##Alt(_T* = nullptr) const                         \
    -> decltype(typename std::enable_if<                                       \
                  !has_static_member_function_##method##Alt<                   \
                    class_of_t<decltype(this)>, void()                         \
                  >::value                                                     \
                >::type(),                                                     \
                non_const_cast(this)->method()) {                              \
  throw std::logic_error("Cannot call '" #method "' without parameters");      \
}                                                                              \
                                                                               \
template<typename _T = void>                                                   \
inline constexpr auto method##Alt(_T* = nullptr)                               \
    -> decltype(typename std::enable_if<                                       \
                  !has_static_member_function_##method##Alt<                   \
                    class_of_t<decltype(this)>, void()                         \
                  >::value                                                     \
                >::type(),                                                     \
                non_const_cast(this)->method()) {                              \
  return static_cast<non_const_return_t<decltype(this->method())>>(            \
    static_cast<const class_of_t<decltype(this)>*>(this)->method##Impl());     \
}                                                                              \
                                                                               \
template<typename _T = void>                                                   \
inline constexpr auto delegate(_T* = nullptr) const                            \
    -> decltype(typename std::enable_if<                                       \
                  !has_static_member_function_delegate<                        \
                    class_of_t<decltype(this)>, void()                         \
                  >::value                                                     \
                >::type(), bool()) {                                           \
  return true;                                                                 \
}                                                                              \
                                                                               \
template<typename _T = void>                                                   \
inline constexpr auto delegate(_T* = nullptr)                                  \
    -> decltype(typename std::enable_if<                                       \
                  !has_static_member_function_delegate<                        \
                    class_of_t<decltype(this)>, void()                         \
                  >::value                                                     \
                >::type(), bool()) {                                           \
  return static_cast<non_const_return_t<decltype(this->method())>>(            \
    static_cast<const class_of_t<decltype(this)>*>(this)->method##Impl());     \
}

/*============================================================================*/
/*                   STATIC MEMBER FUNCTION DELEGATOR CALL                    */
/*============================================================================*/

#define CALL_STATIC_MEMBER_FUNCTION_DELEGATOR(method, ...)                     \
do { return method##Impl(__VA_ARGS__); } while (false)

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_MEMBER_DELEGATOR_
