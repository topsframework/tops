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

#ifndef TOPS_MODEL_MEMBER_DETECTOR_
#define TOPS_MODEL_MEMBER_DETECTOR_

namespace tops {
namespace model {

/*============================================================================*/
/*                            MEMBER TYPE DETECTOR                            */
/*============================================================================*/

#define GENERATE_HAS_MEMBER_TYPE(member)                                       \
                                                                               \
/*- TYPE DETECTOR ----------------------------------------------------------*/ \
                                                                               \
template<typename _Klass, typename _Type>                                      \
class HasType_##member                                                         \
{                                                                              \
 private:                                                                      \
  template<typename _U> static std::true_type test(typename _U::_Type*);       \
  template<typename _U> static std::false_type test(...);                      \
                                                                               \
 public:                                                                       \
  static constexpr bool value = decltype(test<_Klass>(nullptr))::value         \
    || HasType_##member<typename _Klass::Base, _Type>::value;                  \
};                                                                             \
                                                                               \
/*- TAGS -------------------------------------------------------------------*/ \
                                                                               \
struct no_##member##_tag {};                                                   \
struct has_##member##_tag {};                                                  \
                                                                               \
/*- TYPE TRAIT -------------------------------------------------------------*/ \
                                                                               \
template<typename _Klass, typename _Type>                                      \
struct has_type_##member                                                       \
    : public std::integral_constant<                                           \
               bool, HasType_##member<_Klass, _Type>::value> {                 \
  using tag = typename std::conditional<                                       \
                has_type_##member<_Klass, _Type>::value,                       \
                has_##member##_tag, no_##member##_tag                          \
              >::type;                                                         \
}

/*============================================================================*/
/*                          MEMBER FUNCTION DETECTOR                          */
/*============================================================================*/

#define GENERATE_HAS_MEMBER_FUNCTION(member)                                   \
                                                                               \
template<typename _Klass, typename Dummy>                                      \
class HasMemberFunction_##member;                                              \
                                                                               \
/*- NON-CONST MEMBER FUNCTION ----------------------------------------------*/ \
                                                                               \
template<typename _Return, typename... _Args>                                  \
class HasMemberFunction_##member<void, _Return(_Args...)> {                    \
 public:                                                                       \
  static constexpr bool value = false;                                         \
};                                                                             \
                                                                               \
template<typename _Klass, typename _Return, typename... _Args>                 \
class HasMemberFunction_##member<_Klass, _Return(_Args...)>                    \
{                                                                              \
 private:                                                                      \
  template<typename _U, _U> class Check;                                       \
                                                                               \
  template<typename _U>                                                        \
  static std::true_type test(Check<_Return(_U::*)(_Args...), &_U::member>*);   \
                                                                               \
  template<typename _U>                                                        \
  static std::false_type test(...);                                            \
                                                                               \
 public:                                                                       \
  static constexpr bool value = decltype(test<_Klass>(nullptr))::value         \
    || HasMemberFunction_##member<typename _Klass::Base,                       \
                                  _Return(_Args...)>::value;                   \
};                                                                             \
                                                                               \
/*- CONST MEMBER FUNCTION --------------------------------------------------*/ \
                                                                               \
template<typename _Return, typename... _Args>                                  \
class HasMemberFunction_##member<void, const _Return(_Args...)> {              \
 public:                                                                       \
  static constexpr bool value = false;                                         \
};                                                                             \
                                                                               \
template<typename _Klass, typename _Return, typename... _Args>                 \
class HasMemberFunction_##member<_Klass, const _Return(_Args...)>              \
{                                                                              \
 private:                                                                      \
  template<typename _U, _U> class Check;                                       \
                                                                               \
  template<typename _U>                                                        \
  static std::true_type test(                                                  \
    Check<_Return(_U::*)(_Args...) const, &_U::member>*);                      \
                                                                               \
  template<typename _U>                                                        \
  static std::false_type test(...);                                            \
                                                                               \
 public:                                                                       \
  static constexpr bool value = decltype(test<_Klass>(nullptr))::value         \
    || HasMemberFunction_##member<typename _Klass::Base,                       \
                          const _Return(_Args...)>::value;                     \
};                                                                             \
                                                                               \
/*- TAGS -------------------------------------------------------------------*/ \
                                                                               \
struct no_##member##_tag {};                                                   \
struct has_##member##_tag {};                                                  \
                                                                               \
/*- TYPE TRAIT -------------------------------------------------------------*/ \
                                                                               \
template<typename _Klass, typename Dummy>                                      \
struct has_member_function_##member;                                           \
                                                                               \
template<typename _Klass, typename _Return, typename... _Args>                 \
struct has_member_function_##member<_Klass, _Return(_Args...)>                 \
    : public std::integral_constant<bool, HasMemberFunction_##member<          \
                                            _Klass, _Return(_Args...)          \
                                          >::value> {                          \
                                                                               \
  using tag = typename std::conditional<                                       \
                has_member_function_##member<                                  \
                  _Klass, _Return(_Args...)                                    \
                >::value,                                                      \
                has_##member##_tag, no_##member##_tag                          \
              >::type;                                                         \
}

/*============================================================================*/
/*                      STATIC MEMBER FUNCTION DETECTOR                       */
/*============================================================================*/

#define GENERATE_HAS_STATIC_MEMBER_FUNCTION(member)                            \
                                                                               \
template<typename _Klass, typename Dummy>                                      \
class HasStaticMemberFunction_##member;                                        \
                                                                               \
/*- STATIC MEMBER FUNCTION -------------------------------------------------*/ \
                                                                               \
template<typename _Return, typename... _Args>                                  \
class HasStaticMemberFunction_##member<void, _Return(_Args...)> {              \
 public:                                                                       \
  static constexpr bool value = false;                                         \
};                                                                             \
                                                                               \
template<typename _Klass, typename _Return, typename... _Args>                 \
class HasStaticMemberFunction_##member<_Klass, _Return(_Args...)>              \
{                                                                              \
 private:                                                                      \
  template<typename _U, _U> class Check;                                       \
                                                                               \
  template<typename _U>                                                        \
  static std::true_type test(Check<_Return(*)(_Args...), &_U::member>*);       \
                                                                               \
  template<typename _U>                                                        \
  static std::false_type test(...);                                            \
                                                                               \
 public:                                                                       \
  static constexpr bool value = decltype(test<_Klass>(nullptr))::value         \
    || HasStaticMemberFunction_##member<typename _Klass::Base,                 \
                                _Return(_Args...)>::value;                     \
};                                                                             \
                                                                               \
/*- TAGS -------------------------------------------------------------------*/ \
                                                                               \
struct no_##member##_tag {};                                                   \
struct has_##member##_tag {};                                                  \
                                                                               \
/*- TYPE TRAIT -------------------------------------------------------------*/ \
                                                                               \
template<typename _Klass, typename Dummy>                                      \
struct has_static_member_function_##member;                                    \
                                                                               \
template<typename _Klass, typename _Return, typename... _Args>                 \
struct has_static_member_function_##member<_Klass, _Return(_Args...)>          \
    : public std::integral_constant<bool, HasStaticMemberFunction_##member<    \
                                            _Klass, _Return(_Args...)          \
                                          >::value> {                          \
                                                                               \
  using tag = typename std::conditional<                                       \
                has_static_member_function_##member<                           \
                  _Klass, _Return(_Args...)                                    \
                >::value,                                                      \
                has_##member##_tag, no_##member##_tag                          \
              >::type;                                                         \
}

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_MEMBER_DETECTOR_
