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
#include "lang/ModelConfigRegister.hpp"

// Standard headers
#include <memory>
#include <string>
#include <cstddef>

// Internal headers
#include "config/BasicConfig.hpp"

#include "config/Options.hpp"
#include "config/model/ModelConfig.hpp"
#include "config/state/StateConfig.hpp"
#include "config/duration/DurationConfig.hpp"
#include "config/auxiliar/DependencyTreeConfig.hpp"
#include "config/auxiliar/FeatureFunctionLibraryConfig.hpp"

// External headers
#include "chaiscript/language/chaiscript_engine.hpp"

#include "chaiscript/dispatchkit/boxed_value.hpp"
#include "chaiscript/dispatchkit/register_function.hpp"

// Namespace aliases
namespace { namespace co = tops::config::option; }

// Remove!
#include <iostream>

namespace tops {
namespace lang {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

ModelConfigRegister::ModelConfigRegister(chaiscript::ChaiScript &chai)
    : chai_(chai) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

void ModelConfigRegister::startVisit() {
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::endVisit() {
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Size &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Type &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Domain &visited) {
  visited = std::make_shared<typename co::Domain::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Domains &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Alphabet &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Alphabets &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Probability &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Probabilities &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::FeatureFunctions &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
  chai_.add(chaiscript::fun([&visited] (
      const std::string &name, co::FeatureFunction fun) {
    visited.emplace(name, fun);
  }), "feature");
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::OutToInSymbolFunction &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::InToOutSymbolFunction &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Model &visited) {
  visited = std::make_shared<typename co::Model::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Models &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::State &visited) {
  visited = std::make_shared<typename co::State::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::States &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Duration &visited) {
  visited = std::make_shared<typename co::Duration::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::DependencyTree &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::DependencyTrees &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::FeatureFunctionLibrary &visited) {
  visited
    = std::make_shared<typename co::FeatureFunctionLibrary::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::FeatureFunctionLibraries &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}


/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitTag(const std::string &tag,
                                   std::size_t /* count */,
                                   std::size_t /* max */) {
  tag_ = tag;
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitLabel(const std::string &/* label */) {
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitPath(const std::string &/* path */) {
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
}  // namespace tops
