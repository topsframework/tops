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

#ifndef TOPS_MODEL_SIMPLE_LABELER_
#define TOPS_MODEL_SIMPLE_LABELER_

// Standard headers
#include <memory>
#include <utility>

// Internal headers
#include "model/Labeler.hpp"
#include "model/Sequence.hpp"

namespace tops {
namespace model {

// Forward declaration
template<typename Model>
class SimpleLabeler;

/**
 * @typedef SimpleLabelerPtr
 * @brief Alias of pointer to SimpleLabeler.
 */
template<typename Model>
using SimpleLabelerPtr = std::shared_ptr<SimpleLabeler<Model>>;

/**
 * @class SimpleLabeler
 * @brief TODO
 */
template<typename Model>
class SimpleLabeler : public Labeler {
 public:
  // Alias
  using ModelPtr = std::shared_ptr<Model>;

  using Base = Labeler;
  using Self = SimpleLabeler<Model>;
  using SelfPtr = std::shared_ptr<Self>;

  // Static methods
  template<typename... Args>
  static SelfPtr make(Args... args) {
    return std::shared_ptr<Self>(new Self(std::forward<Args>(args)...));
  }

  // Overriden methods
  Estimation<Labeling<Sequence>>
  labeling(const Labeler::method& chosen_method) const override {
    CALL_MEMBER_FUNCTION_DELEGATOR(labeling, chosen_method);
  }

  Sequence& sequence() override {
    return _sequence;
  }

  const Sequence& sequence() const override {
    return _sequence;
  }

 protected:
  // Instace variables
  ModelPtr _model;
  Sequence _sequence;

  // Constructors
  SimpleLabeler(ModelPtr model, Sequence sequence)
      : _model(std::move(model)), _sequence(std::move(sequence)) {
  }

 private:
  GENERATE_MEMBER_FUNCTION_DELEGATOR(labeling, _model)
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_SIMPLE_LABELER_
