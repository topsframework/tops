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
#include "lang/MultipleFilePrinter.hpp"

// Standard headers
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

// Internal headers
#include "lang/Util.hpp"
#include "lang/ModelConfigSerializer.hpp"

#include "config/BasicConfig.hpp"

#include "filesystem/Filesystem.hpp"

// Namespace aliases
namespace { namespace co = tops::config::option; }

namespace tops {
namespace lang {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

MultipleFilePrinter::MultipleFilePrinter(const std::string &root_dir)
  : Base(nullptr, 0), change_ostream_(true),
    root_dir_(cleanPath(root_dir + "/")) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::changeOstream(const std::string &path) {
  working_dir_ = extractDir(extractCorename(path));
  if (change_ostream_) {
    auto new_path = root_dir_ + extractCorename(path);
    filesystem::create_directories(extractDir(new_path));
    os_ = std::make_shared<std::ofstream>(new_path);
  }
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::startPrinting() {
  Base::startPrinting();

  if (change_ostream_) {
    *os_ << "// -*- mode: c++ -*-" << std::endl
         << "// vim: ft=chaiscript:" << std::endl
         << std::endl;
  }
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::endPrinting() {
  Base::endPrinting();

  for (auto &submodel : submodels_)
    printSubmodel(submodel);
  for (auto &library : libraries_)
    printLibrary(library);
  for (auto &tree : trees_)
    printTree(tree);
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(co::Model model) {
  callFunction(model->label(), pathForHelperCall(model->path()));
  submodels_.push_back(model);
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(co::Domain domain) {
  openFunction(domain->data()->label());
  domain->data()->accept(ModelConfigSerializer(
        Self::make(false, root_dir_, os_, depth_, "", ", ", "")));
  closeFunction();
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(co::State state) {
  openSection('[');
  state->accept(ModelConfigSerializer(
        Self::make(false, root_dir_, os_, depth_, ": ", ",\n")));
  closeSection(']');
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(co::Duration duration) {
  openFunction(duration->label());
  duration->accept(ModelConfigSerializer(
        Self::make(false, root_dir_, os_, depth_, "", ", ", "")));
  closeFunction();
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(co::DependencyTree tree) {
  callFunction("tree", pathForHelperCall(tree->path()));
  trees_.push_back(tree);
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(co::FeatureFunctionLibrary library) {
  callFunction("lib", pathForHelperCall(library->path()));
  libraries_.push_back(library);
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

std::string MultipleFilePrinter::pathForHelperCall(const std::string &path) {
  return '"' + removeSubstring(working_dir_, extractCorename(path)) + '"';
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::printSubmodel(co::Model submodel) {
  submodel->accept(ModelConfigSerializer(
        Self::make(true, root_dir_, os_, 0)));
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::printTree(co::DependencyTree tree) {
  thread_local size_t tree_depth = 0;
  thread_local std::vector<size_t> tree_nodes { 1 };

  if (tree_depth == 0)
    changeOstream(tree->path());

  indent();
  for (size_t i = 1; i < tree_nodes.size(); i++) {
    if (i == tree_depth) {
      if (tree_nodes[i] == 1)
        *os_ << " `- ";
      else
        *os_ << " |- ";
    } else {
      if (tree_nodes[i] == 0)
        *os_ << "    ";
      else
        *os_ << " |  ";
    }
  }

  tree_nodes[tree_depth]--;

  if (tree_nodes.size() == tree_depth + 1
  &&  tree->children().size() != 0)
    tree_nodes.push_back(tree->children().size());

  if (tree_nodes.size() == tree_depth + 1
  &&  tree_nodes[tree_depth] == 0)
    tree_nodes.pop_back();

  tree->accept(ModelConfigSerializer(
          Self::make(false, root_dir_, os_, depth_,
                     "", " ", "\n", "(", ")")));

  tree_depth++;

  for (auto& child : tree->children())
    printTree(child);

  tree_depth--;
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::printLibrary(co::FeatureFunctionLibrary library) {
  copy(library, std::make_shared<std::ofstream>(
        root_dir_ + extractCorename(library->path())));
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
}  // namespace tops
