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
#include "lang/DependencyTreeParser.hpp"

// Standard headers
#include <stack>
#include <cctype>
#include <memory>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>

// Internal headers
#include "lang/Interpreter.hpp"

#include "config/BasicConfig.hpp"
#include "config/StringLiteralSuffix.hpp"

// Namespace aliases
namespace { namespace co = tops::config::option; }

// Using declarations
using tops::config::operator ""_t;

namespace tops {
namespace lang {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

DependencyTreeParser::DependencyTreeParser(Interpreter* interpreter,
                                           std::string root_dir,
                                           std::string filename,
                                           std::vector<std::string> content)
    : interpreter_(interpreter),
      root_dir_(root_dir),
      filename_(filename),
      content_(content),
      line_(0), column_(1) {
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

co::DependencyTree DependencyTreeParser::parse() {
  for (auto line : content_) {
    line_++;
    column_ = 1;
    parseNode(line);
  }

  edges_.insert(edges_.begin(), 0);

  std::stack<size_t> stack_edges;
  std::stack<co::DependencyTree> stack_nodes;
  for (size_t i = 0; i < edges_.size(); i++) {
    if (stack_edges.empty()) {
      stack_edges.push(edges_[i]);
      stack_nodes.push(nodes_[i]);
    } else if (stack_edges.top() < edges_[i]) {
      stack_edges.push(edges_[i]);
      stack_nodes.push(nodes_[i]);
    } else if (stack_edges.top() == edges_[i]) {
      auto node1 = stack_nodes.top();
      auto node2 = nodes_[i];
      stack_edges.pop();
      stack_nodes.pop();
      stack_nodes.top()->children().push_back(node1);
      stack_nodes.top()->children().push_back(node2);
    } else {
      auto node = stack_nodes.top();
      stack_nodes.pop();
      stack_edges.pop();
      stack_nodes.top()->children().push_back(node);
      i--;
    }
  }

  while (stack_nodes.size() > 1) {
    auto node = stack_nodes.top();
    stack_nodes.pop();
    stack_nodes.top()->children().push_back(node);
  }
  return stack_nodes.top();
}

/*----------------------------------------------------------------------------*/

void DependencyTreeParser::parseNode(std::string line) {
  it_ = line.begin();

  parseSpaces();

  if (next() != '(') {
    parseLevel();
  }

  consume('(');
  auto id = parseId();
  consume(')');
  consume(' ');
  consume('m');
  consume('o');
  consume('d');
  consume('e');
  consume('l');
  consume('(');
  consume('"');

  auto filepath = parseString();

  auto tree = config::DependencyTreeConfig::make(root_dir_ + filename_);
  std::get<decltype("position"_t)>(*tree) = id;
  std::get<decltype("configuration"_t)>(*tree)
    = interpreter_->evalModelDefinition(root_dir_ + filepath);
  nodes_.push_back(tree);

  consume('"');
  consume(')');
}

/*----------------------------------------------------------------------------*/

void DependencyTreeParser::resetEdgeIndex() {
  reset_edge_index_ = true;
}

/*----------------------------------------------------------------------------*/

size_t DependencyTreeParser::nextEdgeIndex() {
  edge_index_++;

  if (reset_edge_index_) {
    edge_index_ = 0;
    reset_edge_index_ = false;
  }

  while (edge_index_ < leaves_.size()) {
    if (!leaves_[edge_index_])
      return edge_index_;
    edge_index_++;
  }
  return edge_index_;
}

/*----------------------------------------------------------------------------*/

void DependencyTreeParser::parseLevel() {
  parseSpaces();
  resetEdgeIndex();
  while (next() == '|' && next(2) == ' ') {
    if (edges_[nextEdgeIndex()] != column_) {
      std::cerr << "Parse error at (" << line_ << ", " << column_
                << "): Wrong edge position" << std::endl;
      exit(0);
    }
    consume('|');
    parseSpaces();
  }
  edges_.push_back(column_);
  if (next() == '|')
    parseChild();
  else if (next() == '`')
    parseLastChild();
}

/*----------------------------------------------------------------------------*/

void DependencyTreeParser::parseChild() {
  leaves_.push_back(false);
  consume('|');
  consume('-');
  parseSpaces();
}

/*----------------------------------------------------------------------------*/

void DependencyTreeParser::parseLastChild() {
  leaves_.push_back(true);
  consume('`');
  consume('-');
  parseSpaces();
}

/*----------------------------------------------------------------------------*/

int DependencyTreeParser::parseSpaces() {
  int s = 0;
  while (next() == ' ') {
    s++;
    consume();
  }
  return s;
}

/*----------------------------------------------------------------------------*/

std::string DependencyTreeParser::parseId() {
  if (next() == '*') {
    consume();
    return "*";
  } else if (std::isdigit(next())) {
    return parseNumber();
  }

  std::cerr << "Parse error at (" << line_ << ", " << column_
            << "): Node ID should be a number or '*'" << std::endl;
  exit(0);
}

/*----------------------------------------------------------------------------*/

std::string DependencyTreeParser::parseNumber() {
  std::string num = "";
  while (std::isdigit(next())) {
    num += std::string(1, next());
    consume();
  }
  return num;
}

/*----------------------------------------------------------------------------*/

std::string DependencyTreeParser::parseString() {
  std::string str = "";
  while (next() != '"' && next() != '\n') {
    str += std::string(1, next());
    consume();
  }
  return str;
}

/*----------------------------------------------------------------------------*/

void DependencyTreeParser::consume() {
  column_++;
  it_++;
}

/*----------------------------------------------------------------------------*/

void DependencyTreeParser::consume(char c) {
  if (c == next()) {
    column_++;
    it_++;
  } else {
    std::cerr << "Parse error at (" << line_ << ", "
              << column_ << ") :" << std::endl;
    std::cerr << "  Expecting: " << c << std::endl;
    std::cerr << "  Got: " << next() <<std::endl;
    exit(0);
  }
}

/*----------------------------------------------------------------------------*/

char DependencyTreeParser::next() {
  return *it_;
}

/*----------------------------------------------------------------------------*/

char DependencyTreeParser::next(int n) {
  return *(it_ + n - 1);
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
}  // namespace tops
