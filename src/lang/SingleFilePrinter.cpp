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
#include "lang/SingleFilePrinter.hpp"

// Standard headers
#include <memory>
#include <algorithm>

// Internal headers
#include "lang/ModelConfigSerializer.hpp"

#include "config/BasicConfig.hpp"

// Namespace aliases
namespace { namespace co = tops::config::option; }

namespace tops {
namespace lang {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

SingleFilePrinter::SingleFilePrinter(std::shared_ptr<std::ostream> os)
  : Base(os, 0) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

void SingleFilePrinter::print(co::Model model) {
  openFunction(model->label());
  openSection('{');
  model->accept(ModelConfigSerializer(Self::make(os_, depth_)));
  closeSection('}');
  closeFunction();
}

/*----------------------------------------------------------------------------*/

void SingleFilePrinter::print(co::Domain domain) {
  openFunction(domain->data()->label());
  domain->data()->accept(ModelConfigSerializer(
        Self::make(os_, depth_, "", ", ", "")));
  closeFunction();
}

/*----------------------------------------------------------------------------*/

void SingleFilePrinter::print(co::State state) {
  openSection('[');
  state->accept(ModelConfigSerializer(
        Self::make(os_, depth_, ": ", ",\n")));
  closeSection(']');
}

/*----------------------------------------------------------------------------*/

void SingleFilePrinter::print(co::Duration duration) {
  openFunction(duration->label());
  duration->accept(ModelConfigSerializer(
        Self::make(os_, depth_, "", ", ", "")));
  closeFunction();
}

/*----------------------------------------------------------------------------*/

void SingleFilePrinter::print(co::DependencyTree tree) {
  thread_local size_t depth_tree = 0;

  if (depth_tree == 0)
    openFunction("tree");

  tree->accept(ModelConfigSerializer(
        Self::make(os_, depth_ + depth_tree, "", ", ", "\n")));

  depth_tree++;
  for (auto& child : tree->children()) {
    indent(depth_tree);
    print(child);
  }
  depth_tree--;

  if (depth_tree == 0) {
    indent(depth_tree);
    closeFunction();
  }
}

/*----------------------------------------------------------------------------*/

void SingleFilePrinter::print(co::FeatureFunctionLibrary library) {
  openSection('{');
  copy(library, os_);
  closeSection('}');
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
}  // namespace tops
