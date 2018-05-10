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

#ifndef CONFIG_DOMAIN_
#define CONFIG_DOMAIN_

// Standard headers
#include <memory>
#include <iostream>

// Internal headers
#include "config/Options.hpp"
#include "config/ConfigWithOptions.hpp"

#include "config/Converter.hpp"

namespace tops {
namespace config {

// Forward declarations
class Domain;

/**
 * @typedef DomainPtr
 * @brief Alias of pointer to Domain
 */
using DomainPtr = std::shared_ptr<Domain>;

/**
 * @class Domain
 * @brief Class representing a domain of an input sequence
 */
class Domain {
 public:
  // Tags
  class discrete_domain {};
  class custom_domain {};

  // Alias
  using Data = config_with_options<>::type<class DataID>;
  using DataPtr = std::shared_ptr<Data>;

  // Constructors
  Domain() = default;

  Domain(discrete_domain, option::Alphabet alphabet);

  Domain(custom_domain, option::OutToInSymbolFunction out_to_in,
                        option::InToOutSymbolFunction in_to_out);

  // Concrete methods
  ConverterPtr makeConverter() const;
  std::shared_ptr<Data> data();

  // Destructor
  virtual ~Domain() = default;

 private:
  // Aliases
  using DiscreteDomainData = config_with_options<
    option::Alphabet(decltype("alphabet"_t))
  >::extending<Data>::type<class DiscreteDomainDataID>;

  using CustomDomainData = config_with_options<
    option::OutToInSymbolFunction(decltype("out_to_in"_t)),
    option::InToOutSymbolFunction(decltype("in_to_out"_t))
  >::extending<Data>::type<class CustomDomainDataID>;

  // Instance variables
  ConverterPtr converter_;
  DataPtr data_;
};

}  // namespace config
}  // namespace tops

namespace tops {
namespace config {
namespace option {

using Domain = DomainPtr;
using Domains = std::vector<DomainPtr>;

}  // namespace option
}  // namespace config
}  // namespace tops

#endif  // CONFIG_DOMAIN_
