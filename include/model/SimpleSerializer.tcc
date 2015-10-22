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

#ifndef TOPS_MODEL_SIMPLE_SERIALIZER_
#define TOPS_MODEL_SIMPLE_SERIALIZER_

// Standard headers
#include <memory>
#include <typeinfo>
#include <exception>

// ToPS templates
#include "model/Serializer.tcc"
#include "model/MemberDelegator.tcc"

namespace tops {
namespace model {

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                    CLASS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

template<typename Model>
class SimpleSerializer;

/**
 * @typedef SimpleSerializerPtr
 * @brief Alias of pointer to SimpleSerializer.
 */
template<typename Model>
using SimpleSerializerPtr = std::shared_ptr<SimpleSerializer<Model>>;

/**
 * @class SimpleSerializer
 * @brief TODO
 */
template<typename Model>
class SimpleSerializer : public Serializer {
 public:
  // Alias
  using ModelPtr = std::shared_ptr<Model>;

  using Self = SimpleSerializer<Model>;
  using SelfPtr = std::shared_ptr<Self>;

  // Static methods
  template<typename... Ts>
  static SelfPtr make(Ts... args) {
    return std::shared_ptr<Self>(new Self(std::forward<Ts>(args)...));
  }

  // Overriden methods
  void serialize(const Serializer::traversal &type) override {
    CALL_MEMBER_FUNCTION_DELEGATOR(serialize, type);
  }

  TranslatorPtr translator() override {
    return _translator;
  }

 protected:
  // Instance variables
  ModelPtr _model;
  TranslatorPtr _translator;

  // Constructors
  SimpleSerializer(ModelPtr m, TranslatorPtr translator)
      : _model(std::move(m)), _translator(std::move(translator)) {
  }

 private:
  GENERATE_MEMBER_FUNCTION_DELEGATOR(serialize, _model)
};

}  // namespace model
}  // namespace tops

#endif
