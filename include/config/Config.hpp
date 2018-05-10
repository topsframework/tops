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

#ifndef CONFIG_BASIC_CONFIG_INTERFACE_
#define CONFIG_BASIC_CONFIG_INTERFACE_

// Standard headers
#include <memory>
#include <string>
#include <cstddef>

#include "config/Util.hpp"

namespace tops {
namespace config {

// Forward declarations
class Config;
class ConfigVisitor;

/**
 * @class ConfigPtr
 * @brief Pointer to config::Config
 */
using ConfigPtr = std::shared_ptr<Config>;

/**
 * @class Config
 * Base class for config::BasicConfig hierarchy representing a config IR
 */
class Config
    : public std::enable_shared_from_this<Config> {
 public:
  // Alias
  using Self = Config;

  // Constructors
  Config(const std::string &path,
                       const std::string &label);

  // Purely virtual methods
  virtual void accept(ConfigVisitor &/* visitor */) const = 0;
  virtual void accept(ConfigVisitor &&/* visitor */) const = 0;

  // Virtual methods
  virtual std::string path();
  virtual const std::string path() const;

  virtual std::string label();
  virtual const std::string label() const;

  virtual std::size_t number_of_options() const;

  // Concrete methods
  template<typename Func>
  void for_each(Func&& /* func */) const;

  template<typename... Args>
  void initialize(Args&&... /* args */) const;

  template<class Tag>
  constexpr decltype(auto) get() const;

  // Destructor
  virtual ~Config() = default;

 private:
  std::string path_;
  std::string label_;
};

}  // namespace config
}  // namespace tops

// Implementation header
#include "config/Config.ipp"

#endif  // CONFIG_BASIC_CONFIG_INTERFACE_
