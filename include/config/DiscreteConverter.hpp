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

#ifndef CONFIG_DISCRETE_CONVERTER_
#define CONFIG_DISCRETE_CONVERTER_

// Standard headers
#include <map>
#include <memory>

// Internal headers
#include "config/Options.hpp"
#include "config/Converter.hpp"

#include "model/Symbol.hpp"

namespace tops {
namespace config {

// Forward declarations
class DiscreteConverter;

/**
 * @typedef DiscreteConverterPtr
 * @brief Alias of pointer to DiscreteConverter
 */
using DiscreteConverterPtr = std::shared_ptr<DiscreteConverter>;

/**
 * @class DiscreteConverter
 * @brief Class to convert outter discrete alphabet to inner alphabet
 */
class DiscreteConverter : public Converter {
 public:
  // Constructors
  explicit DiscreteConverter(option::Alphabet alphabet);

  // Overriden methods
  model::Symbol convert(const option::Symbol &orig) const override;
  option::Symbol convert(const model::Symbol &orig) const override;

  // Concrete methods
  option::Alphabet& alphabet();
  const option::Alphabet& alphabet() const;

 private:
  // Instance variables
  option::Alphabet alphabet_;
  std::map<model::Symbol, option::Symbol> in_to_out_;
  std::map<option::Symbol, model::Symbol> out_to_in_;
};

}  // namespace config
}  // namespace tops

#endif  // CONFIG_DISCRETE_CONVERTER_
