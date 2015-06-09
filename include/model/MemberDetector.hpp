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

#define GENERATE_HAS_MEMBER_METHOD(member)                                     \
                                                                               \
template<typename T, typename Dummy>                                           \
class HasMethod_##member;                                                      \
                                                                               \
/** NON-CONST MEMBER ********************************************************/ \
                                                                               \
template<typename Result, typename... Params>                                  \
class HasMethod_##member<void, Result(Params...)> {                            \
 public:                                                                       \
  static constexpr bool value = false;                                         \
};                                                                             \
                                                                               \
template<typename T, typename Result, typename... Params>                      \
class HasMethod_##member<T, Result(Params...)>                                 \
{                                                                              \
 private:                                                                      \
  template<typename U, U> class Check;                                         \
                                                                               \
  template<typename U>                                                         \
  static std::true_type test(Check<Result(U::*)(Params...), &U::member>*);     \
                                                                               \
  template<typename U>                                                         \
  static std::false_type test(...);                                            \
                                                                               \
 public:                                                                       \
  static constexpr bool value = decltype(test<T>(nullptr))::value              \
    || HasMethod_##member<typename T::Base, Result(Params...)>::value;         \
};                                                                             \
                                                                               \
/** CONST MEMBER ************************************************************/ \
                                                                               \
template<typename Result, typename... Params>                                  \
class HasMethod_##member<void, const Result(Params...)> {                      \
 public:                                                                       \
  static constexpr bool value = false;                                         \
};                                                                             \
                                                                               \
template<typename T, typename Result, typename... Params>                      \
class HasMethod_##member<T, const Result(Params...)>                           \
{                                                                              \
 private:                                                                      \
  template<typename U, U> class Check;                                         \
                                                                               \
  template<typename U>                                                         \
  static std::true_type test(                                                  \
    Check<Result(U::*)(Params...) const, &U::member>*);                        \
                                                                               \
  template<typename U>                                                         \
  static std::false_type test(...);                                            \
                                                                               \
 public:                                                                       \
  static constexpr bool value = decltype(test<T>(nullptr))::value              \
    || HasMethod_##member<typename T::Base, const Result(Params...)>::value;   \
};                                                                             \
                                                                               \
/** TAGS ********************************************************************/ \
                                                                               \
struct no_##member##_tag {};                                                   \
struct has_##member##_tag {};                                                  \
                                                                               \
/** TYPE TRAIT **************************************************************/ \
                                                                               \
template<typename T, typename Dummy>                                           \
struct has_member_##member;                                                    \
                                                                               \
template<typename T, typename Result, typename... Params>                      \
struct has_member_##member<T, Result(Params...)>                               \
    : public std::integral_constant<                                           \
               bool, HasMethod_##member<T, Result(Params...)>::value> {        \
                                                                               \
  using tag = typename std::conditional<                                       \
                has_member_##member<T, Result(Params...)>::value,              \
                has_##member##_tag, no_##member##_tag                          \
              >::type;                                                         \
};

}  // namespace model
}  // namespace tops

#endif
