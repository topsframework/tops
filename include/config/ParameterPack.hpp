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

/**
 * @file ParameterPack.hpp
 * Implementation based on http://stackoverflow.com/questions/14261183/
 */

// Standard headers
#include <utility>
#include <type_traits>

namespace tops {
namespace config {

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                               FIRST/LAST/N-TH TYPE
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

// Declare primary template
template<std::size_t I, typename... Ts>
struct nth_type_of {
};

// Base step
template<typename T, typename... Ts>
struct nth_type_of<0, T, Ts...> {
  using type = T;
};

// Induction step
template<std::size_t I, typename T, typename... Ts>
struct nth_type_of<I, T, Ts...> {
  using type = typename nth_type_of<I - 1, Ts...>::type;
};

// Helper meta-function for retrieving the first type in a parameter pack
template<typename... Ts>
struct first_type_of {
  using type = typename nth_type_of<0, Ts...>::type;
};

// Helper meta-function for retrieving the last type in a parameter pack
template<typename... Ts>
struct last_type_of {
  using type = typename nth_type_of<sizeof...(Ts) - 1, Ts...>::type;
};

// Alias
template<std::size_t I, typename... Ts>
using nth_type_of_t = typename nth_type_of<I, Ts...>::type;

template<typename... Ts>
using first_type_of_t = typename first_type_of<Ts...>::type;

template<typename... Ts>
using last_type_of_t = typename last_type_of<Ts...>::type;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                              FIRST/LAST/N-TH VALUE
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

// Base step
template<std::size_t I, typename T, typename... Ts>
auto nth_value_of(T&& t, Ts&&... /* args */)
    -> std::enable_if_t<(I == 0), decltype(std::forward<T>(t))> {
  return std::forward<T>(t);
}

// Induction step
template<std::size_t I, typename T, typename... Ts>
auto nth_value_of(T&& /* t */, Ts&&... args)
    -> std::enable_if_t<
         (I > 0), decltype(std::forward<nth_type_of_t<I, T, Ts...>>(
                             std::declval<nth_type_of_t<I, T, Ts...>>()))> {
  return std::forward<nth_type_of_t<I, T, Ts...>>(
    nth_value_of<I - 1>((std::forward<Ts>(args))...));
}

// Helper function for retrieving the first value of an argument pack
template<typename... Ts>
decltype(auto) first_value_of(Ts&&... args) {
  return std::forward<first_type_of_t<Ts...>>(
    nth_value_of<0>((std::forward<Ts>(args))...));
}

// Helper function for retrieving the last value of an argument pack
template<typename... Ts>
decltype(auto) last_value_of(Ts&&... args) {
  return std::forward<last_type_of_t<Ts...>>(
    nth_value_of<sizeof...(Ts) - 1>((std::forward<Ts>(args))...));
}

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                   INDEX LISTS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

// Collects internal details for generating index ranges [Min, Max)
namespace detail {

// Declare primary template for index range builder
template<std::size_t Min, std::size_t N, std::size_t... Is>
struct range_builder;

// Base step
template<std::size_t Min, std::size_t... Is>
struct range_builder<Min, Min, Is...> {
  using type = std::index_sequence<Is...>;
};

// Induction step
template<std::size_t Min, std::size_t N, std::size_t... Is>
struct range_builder : public range_builder<Min, N - 1, N - 1, Is...> {
};

}  // namespace detail

// Meta-function that returns a [Min, Max) index range
template<std::size_t Min, std::size_t Max>
using index_range = typename detail::range_builder<Min, Max>::type;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                      LOOPS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

// Implementation inspired by @jogojapan's answer to this question:
// http://stackoverflow.com/questions/14089637/

// Collects internal details for implementing functor invocation
namespace detail {

// Functor invocation is realized through variadic inheritance.
// The constructor of each base class invokes an input functor.
// An functor invoker for an argument pack has one base class
// for each argument in the pack

// Realizes the invocation of the functor for one parameter
template<std::size_t I, typename T>
struct invoker_base {
  template<typename F, typename U>
  invoker_base(F&& f, U&& u) { f(u); }
};

// Necessary because a class cannot inherit the same class twice
template<std::size_t I, typename T>
struct indexed_type {
  static const std::size_t index = I;
  using type = T;
};

// The functor invoker: inherits from a list of base classes.
// The constructor of each of these classes invokes the input
// functor with one of the arguments in the pack.
template<typename... Ts>
struct invoker : public invoker_base<Ts::index, typename Ts::type>... {
  template<typename F, typename... Us>
  invoker(F&& f, Us&&... args)
      : invoker_base<Ts::index, typename Ts::type>(
          std::forward<F>(f), std::forward<Us>(args))... {
  }
};

}  // namespace detail

// The functor provided in the first argument is invoked for each
// argument in the pack whose index is contained in the index list
// specified in the second argument
template<typename F, std::size_t... Is, typename... Ts>
void for_each_in_arg_pack_subset(
    F&& f, std::index_sequence<Is...> const& /* i */, Ts&&... args) {
  // Constructors of invoker's sub-objects will invoke the functor.
  // Note that argument types must be paired with numbers because the
  // implementation is based on inheritance, and one class cannot
  // inherit the same base class twice.
  detail::invoker<detail::indexed_type<Is, nth_type_of_t<Is, Ts...>>...>
  invoker(f, (nth_value_of<Is>(std::forward<Ts>(args)...))...);
}

// The functor provided in the first argument is invoked for each
// argument in the pack
template<typename F, typename... Ts>
void for_each_in_arg_pack(F&& f, Ts&&... args) {
  for_each_in_arg_pack_subset(
    f, index_range<0, sizeof...(Ts)>(), std::forward<Ts>(args)...);
}

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                   FORWARDING
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

// The functor provided in the first argument is given in input the
// arguments in whose index is contained in the index list specified
// as the second argument.
template<typename F, std::size_t... Is, typename... Ts>
void forward_subpack(
    F&& f, std::index_sequence<Is...> const& /* i */, Ts&&... args) {
  f((nth_value_of<Is>(std::forward<Ts>(args)...))...);
}

// The functor provided in the first argument is given in input all the
// arguments in the pack.
template<typename F, typename... Ts>
void forward_pack(F&& f, Ts&&... args) {
  f(std::forward<Ts>(args)...);
}

}  // namespace config
}  // namespace tops
