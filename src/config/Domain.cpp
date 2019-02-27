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

// Interface header
#include "config/Domain.hpp"

// Standard headers
#include <memory>
#include <utility>

// Internal headers
#include "config/BasicConfig.hpp"
#include "config/CustomConverter.hpp"
#include "config/DiscreteConverter.hpp"

namespace tops {
namespace config {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

Domain::Domain(discrete_domain, option::Alphabet alphabet)
    : converter_(std::make_shared<DiscreteConverter>(std::move(alphabet))) {
  auto data = std::make_shared<DiscreteDomainData>("", "discrete_domain");
  std::get<decltype("alphabet"_t)>(*data) = std::move(alphabet);
  data_ = data;
}

/*----------------------------------------------------------------------------*/

Domain::Domain(custom_domain, option::OutToInSymbolFunction out_to_in,
                              option::InToOutSymbolFunction in_to_out)
    : converter_(std::make_shared<CustomConverter>(
          std::move(out_to_in), std::move(in_to_out))) {
  auto data = std::make_shared<CustomDomainData>("", "custom_domain");
  std::get<decltype("out_to_in"_t)>(*data) = std::move(out_to_in);
  std::get<decltype("in_to_out"_t)>(*data) = std::move(in_to_out);
  data_ = data;
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

ConverterPtr Domain::makeConverter() const {
  return converter_;
}

/*----------------------------------------------------------------------------*/

std::shared_ptr<typename Domain::Data> Domain::data() {
  return data_;
}

/*----------------------------------------------------------------------------*/

}  // namespace config
}  // namespace tops
