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

#ifndef CONFIG_CONVERTER_
#define CONFIG_CONVERTER_

// Standard headers
#include <memory>

// Internal headers
#include "config/Options.hpp"

#include "model/Symbol.hpp"

namespace tops {
namespace config {

// Forward declarations
class Converter;

/**
 * @typedef ConverterPtr
 * @brief Alias of pointer to Converter
 */
using ConverterPtr = std::shared_ptr<Converter>;

/**
 * @class Converter
 * @brief Class to convert outter to/from inner symbols
 */
class Converter {
 public:
  // Purely virtual methods
  virtual model::Symbol convert(const option::Symbol &orig) const = 0;
  virtual option::Symbol convert(const model::Symbol &orig) const = 0;

  // Destructor
  virtual ~Converter() = default;
};

}  // namespace config
}  // namespace tops

#endif  // CONFIG_CONVERTER_
