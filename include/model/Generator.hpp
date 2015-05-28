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

#ifndef TOPS_MODEL_GENERATOR_
#define TOPS_MODEL_GENERATOR_

// Standard headers
#include <memory>

// ToPS headers
#include "model/Sequence.hpp"

namespace tops {
namespace model {

class ProbabilisticModel;
using ProbabilisticModelPtr = std::shared_ptr<ProbabilisticModel>;

class Generator;

/**
 * @typedef GeneratorPtr
 * @brief Alias of pointer to Generator.
 */
using GeneratorPtr = std::shared_ptr<Generator>;

/**
 * @class Generator
 * @brief TODO
 */
class Generator : public std::enable_shared_from_this<Generator> {
 public:
  // Static methods
  template<typename... Ts>
  static GeneratorPtr make(Ts... args) {
    return GeneratorPtr(new Generator(std::forward<Ts>(args)...));
  }

  // Virtual methods
  virtual Sequence choose(unsigned int size,
                          unsigned int phase = 0) const;

 protected:
  Generator(ProbabilisticModelPtr &&model);

  ProbabilisticModelPtr _model;
};

}  // namespace model
}  // namespace tops

#endif
