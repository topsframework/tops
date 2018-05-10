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

#ifndef CONFIG_BASIC_CONFIG_
#define CONFIG_BASIC_CONFIG_

// Standard headers
#include <memory>
#include <string>
#include <vector>
#include <cstddef>
#include <utility>
#include <type_traits>

// Internal headers
#include "config/HasTag.hpp"
#include "config/ParameterPack.hpp"
#include "config/Config.hpp"

#include "config/Util.hpp"

// External headers
#include "named_types/named_tuple.hpp"

namespace tops {
namespace config {

// Forward declarations
class ConfigVisitor;

/**
 * @class BasicConfig
 * @brief General class to create an intermediate representations (IR)
 * @tparam Base    Base class for the config IR
 * @tparam ID      Unique identifier to differentiate configs
 * @tparam Options List of Type(decltype("name"_t)) to be stored in config IR
 */
template<typename ID, typename Base, typename... Options>
class BasicConfig : public Base {
 public:
  // Alias
  using Self = BasicConfig<ID, Base, Options...>;
  using Tuple = named_types::named_tuple<Options...>;

  using SelfPtr = std::shared_ptr<BasicConfig<ID, Base, Options...>>;

  // Constructors
  explicit BasicConfig(const std::string &path = {},
                       const std::string &label = {});

  // Static methods
  template<typename... Params>
  static SelfPtr make(Params&&... params);

  // Overriden methods
  void accept(ConfigVisitor &visitor) const override;
  void accept(ConfigVisitor &&visitor) const override;

  std::size_t number_of_options() const override;

  // Concrete methods
  template<typename Func>
  void for_each(Func&& func) const;

  template<typename... Args>
  void initialize(Args&&... args);

  template<class Tag>
  constexpr decltype(auto)
  get(std::enable_if_t<!has_tag<Tag, Options...>()>* = nullptr) const &;

  template<class Tag>
  constexpr decltype(auto)
  get(std::enable_if_t<has_tag<Tag, Options...>()>* = nullptr) const &;

  template<class Tag>
  decltype(auto)
  get(std::enable_if_t<!has_tag<Tag, Options...>()>* = nullptr) &;

  template<class Tag>
  decltype(auto)
  get(std::enable_if_t<has_tag<Tag, Options...>()>* = nullptr) &;

  template<class Tag>
  decltype(auto)
  get(std::enable_if_t<!has_tag<Tag, Options...>()>* = nullptr) &&;

  template<class Tag>
  decltype(auto)
  get(std::enable_if_t<has_tag<Tag, Options...>()>* = nullptr) &&;

  std::vector<SelfPtr> &children();
  const std::vector<SelfPtr> &children() const;

 private:
  // Instance variables
  Tuple attrs_;
  std::vector<SelfPtr> children_;

  // Concrete methods
  template<typename... Args>
  void initialize_base(Args&&... args);

  template<typename... Args>
  void initialize_tuple(Args&&... args);
};

}  // namespace config
}  // namespace tops

namespace std {

/**
 * @fn get
 * @brief Function overload to get option stored in tops::config::BasicConfig
 * @tparam Tag String literal to be looked up in list of Options
 * @tparam ID Unique identified of the IR
 * @tparam Base Base class for the config IR
 * @tparam Options List of Type(decltype("name"_t)) to be stored in config IR
 */
template<typename Tag, typename ID, typename Base, typename... Options>
decltype(auto) get(tops::config::BasicConfig<ID, Base, Options...> const &input) {
  return input.template get<Tag>();
}

/**
 * @fn get
 * @brief Function overload to get option stored in tops::config::BasicConfig
 * @tparam Tag String literal to be looked up in list of Options
 * @tparam ID Unique identified of the IR
 * @tparam Base Base class for the config IR
 * @tparam Options List of Type(decltype("name"_t)) to be stored in config IR
 */
template<typename Tag, typename ID, typename Base, typename... Options>
decltype(auto) get(tops::config::BasicConfig<ID, Base, Options...> &input) {
  return input.template get<Tag>();
}

/**
 * @fn get
 * @brief Function overload to get option stored in tops::config::BasicConfig
 * @tparam Tag String literal to be looked up in list of Options
 * @tparam ID Unique identified of the IR
 * @tparam Base Base class for the config IR
 * @tparam Options List of Type(decltype("name"_t)) to be stored in config IR
 */
template<typename Tag, typename ID, typename Base, typename... Options>
decltype(auto) get(tops::config::BasicConfig<ID, Base, Options...> &&input) {
  return move(input).template get<Tag>();
}

}  // namespace std

// Implementation header
#include "config/BasicConfig.ipp"

#endif  // CONFIG_BASIC_CONFIG_
