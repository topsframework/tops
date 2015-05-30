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

#define GENERATE_HAS_MEMBER(member)                                     \
                                                                        \
template < class T >                                                    \
class HasMember_##member                                                \
{                                                                       \
private:                                                                \
    using Yes = char[2];                                                \
    using  No = char[1];                                                \
                                                                        \
    struct Fallback { int member; };                                    \
    struct Derived : T, Fallback { };                                   \
                                                                        \
    template < class U >                                                \
    static No& test ( decltype(U::member)* );                           \
    template < typename U >                                             \
    static Yes& test ( U* );                                            \
                                                                        \
public:                                                                 \
    static constexpr bool RESULT                                        \
      = sizeof(test<Derived>(nullptr)) == sizeof(Yes);                  \
};                                                                      \
                                                                        \
template < class T >                                                    \
struct has_member_##member                                              \
: public std::integral_constant<bool, HasMember_##member<T>::RESULT>    \
{ };                                                                    \
                                                                        \
template<typename Model>                                                \
using has_##member = typename                                           \
  std::enable_if<has_member_##member<Model>::value, bool>::type;        \
                                                                        \
template<typename Model>                                                \
using no_##member = typename                                            \
  std::enable_if<!has_member_##member<Model>::value, bool>::type;

}  // namespace model
}  // namespace tops

#endif
