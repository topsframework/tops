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

#ifndef LANG_DEPENDENCY_TREE_PARSER_
#define LANG_DEPENDENCY_TREE_PARSER_

// Standard headers
#include <string>
#include <vector>

// Internal headers
#include "config/Options.hpp"
#include "config/model/ModelConfig.hpp"
#include "config/auxiliar/DependencyTreeConfig.hpp"

namespace tops {
namespace lang {

// Namespace aliases
namespace { namespace co = tops::config::option; }

// Forward declarations
class Interpreter;

/**
 * @class DependencyTreeParser
 * @brief Parser to dependency tree serialized file
 */
class DependencyTreeParser {
 public:
  // Constructors
  DependencyTreeParser(Interpreter* interpreter,
                       std::string root_dir,
                       std::string filename,
                       std::vector<std::string> content);

  // Concrete methods
  co::DependencyTree parse();

 private:
  // Concrete methods
  void parseNode(std::string line);

  void resetEdgeIndex();
  size_t nextEdgeIndex();

  void parseLevel();
  void parseChild();
  void parseLastChild();

  int parseSpaces();
  std::string parseId();
  std::string parseNumber();
  std::string parseString();

  void consume();
  void consume(char c);

  char next();
  char next(int n);

  // Instance variables
  Interpreter* interpreter_;

  std::string root_dir_;
  std::string filename_;

  std::vector<std::string> content_;
  std::string::iterator it_;

  std::vector<size_t> edges_;
  std::vector<bool> leaves_;
  std::vector<co::DependencyTree> nodes_;

  size_t line_;
  size_t column_;
  size_t edge_index_;

  bool reset_edge_index_;
};

}  // namespace lang
}  // namespace tops

#endif  // LANG_DEPENDENCY_TREE_PARSER_
