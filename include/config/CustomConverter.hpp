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

#ifndef CONFIG_CUSTOM_CONVERTER_
#define CONFIG_CUSTOM_CONVERTER_

// Standard headers
#include <memory>
#include <functional>

// Internal headers
#include "config/Options.hpp"
#include "config/Converter.hpp"

#include "model/Symbol.hpp"

namespace tops {
namespace config {

// Forward declarations
class CustomConverter;

/**
 * @typedef DiscreteConverterPtr
 * @brief Alias of pointer to CustomConverter
 */
using CustomConverterPtr = std::shared_ptr<CustomConverter>;

/**
 * @class CustomConverter
 * @brief Class to convert outter to inner alphabet with user-provided functions
 */
class CustomConverter : public Converter {
 public:
  // Aliases
  using OutToInFunction = std::function<model::Symbol(const option::Symbol&)>;
  using InToOutFunction = std::function<option::Symbol(const model::Symbol&)>;

  // Constructors
  explicit CustomConverter(OutToInFunction out_to_in,
                           InToOutFunction in_to_out);

  // Overriden methods
  model::Symbol convert(const option::Symbol &orig) const override;
  option::Symbol convert(const model::Symbol &orig) const override;

 private:
  // Instance variables
  OutToInFunction out_to_in_;
  InToOutFunction in_to_out_;
};

}  // namespace config
}  // namespace tops

#endif  // CONFIG_CUSTOM_CONVERTER_
