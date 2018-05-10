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

#ifndef CONFIG_CONFIG_VISITOR_
#define CONFIG_CONFIG_VISITOR_

// Standard headers
#include <memory>
#include <string>
#include <type_traits>

#include "config/Domain.hpp"
#include "config/Options.hpp"

#include "config/model/ModelConfig.hpp"
#include "config/state/StateConfig.hpp"
#include "config/duration/DurationConfig.hpp"
#include "config/auxiliar/DependencyTreeConfig.hpp"
#include "config/auxiliar/FeatureFunctionLibraryConfig.hpp"

namespace tops {
namespace config {

// Forward declaration
class Config;

/**
 * @class ConfigVisitor
 * @brief Visitor for config::BasicConfig's options
 */
class ConfigVisitor {
 public:
  // Concrete methods
  template<typename ID, typename Base, typename... Options>
  void visit(std::shared_ptr<BasicConfig<ID, Base, Options...>> config) {
    std::size_t count = 0;
    std::size_t max = config->number_of_options();

    this->visitLabel(config->label());
    this->visitPath(config->path());

    this->startVisit();
    config->for_each([this, &count, &max] (const auto &tag, auto &value) {
      using Tag = std::decay_t<decltype(tag)>;
      using Value = std::decay_t<decltype(value)>;
      this->visitTag(typename Tag::value_type().str(), count, max);
      this->visitOption(const_cast<Value&>(value));
      count++;
    });
    this->endVisit();
  }

  // Virtual destructor
  virtual ~ConfigVisitor() = default;

 protected:
  // Purely virtual methods
  virtual void startVisit() = 0;
  virtual void endVisit() = 0;

  virtual void visitTag(const std::string &, std::size_t, std::size_t) = 0;
  virtual void visitLabel(const std::string &) = 0;
  virtual void visitPath(const std::string &) = 0;

  virtual void visitOption(option::Type &) = 0;
  virtual void visitOption(option::Size &) = 0;
  virtual void visitOption(option::Domain &) = 0;
  virtual void visitOption(option::Domains &) = 0;
  virtual void visitOption(option::Alphabet &) = 0;
  virtual void visitOption(option::Alphabets &) = 0;
  virtual void visitOption(option::Probability &) = 0;
  virtual void visitOption(option::Probabilities &) = 0;
  virtual void visitOption(option::FeatureFunctions &) = 0;
  virtual void visitOption(option::OutToInSymbolFunction &) = 0;
  virtual void visitOption(option::InToOutSymbolFunction &) = 0;

  virtual void visitOption(option::Model &) = 0;
  virtual void visitOption(option::Models &) = 0;
  virtual void visitOption(option::State &) = 0;
  virtual void visitOption(option::States &) = 0;
  virtual void visitOption(option::Duration &) = 0;
  virtual void visitOption(option::DependencyTree &) = 0;
  virtual void visitOption(option::DependencyTrees &) = 0;
  virtual void visitOption(option::FeatureFunctionLibrary &) = 0;
  virtual void visitOption(option::FeatureFunctionLibraries &) = 0;
};

}  // namespace config
}  // namespace tops

#endif  // CONFIG_CONFIG_VISITOR_
