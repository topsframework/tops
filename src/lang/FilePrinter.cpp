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
#include "lang/FilePrinter.hpp"

// Standard header
#include <memory>
#include <string>
#include <ostream>
#include <algorithm>

namespace tops {
namespace lang {

/*----------------------------------------------------------------------------*/
/*                              VIRTUAL METHODS                               */
/*----------------------------------------------------------------------------*/

void FilePrinter::changeOstream(const std::string &/* path */) {
}

/*----------------------------------------------------------------------------*/

void FilePrinter::startPrinting() {
}

/*----------------------------------------------------------------------------*/

void FilePrinter::endPrinting() {
  *os_ << option_.end;
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

void FilePrinter::print(const std::string &string) {
  *os_ << option_.string_start
       << string
       << option_.string_end;
}

/*----------------------------------------------------------------------------*/

void FilePrinter::print(float num) {
  *os_ << std::fixed << num;
}

/*----------------------------------------------------------------------------*/

void FilePrinter::print(double num) {
  *os_ << std::fixed << num;
}

/*----------------------------------------------------------------------------*/

void FilePrinter::print(size_t num) {
  *os_ << num;
}

/*----------------------------------------------------------------------------*/

void FilePrinter::print(bool boolean) {
  *os_ << std::boolalpha << boolean;
}

/*----------------------------------------------------------------------------*/

void FilePrinter::printTag(const std::string &tag, size_t count, size_t max) {
  if (count > 0 && count < max) {
    *os_ << option_.middle;
  }
  if (!option_.attribution.empty()) {
    indent();
    *os_ << tag << option_.attribution;
  }
}

/*----------------------------------------------------------------------------*/

void FilePrinter::callFunction(const std::string &name,
                               const std::string &content) {
  openFunction(name);
  *os_ << content;
  closeFunction();
}

/*----------------------------------------------------------------------------*/

void FilePrinter::openFunction(const std::string name) {
  *os_ << name << "(";
}

/*----------------------------------------------------------------------------*/

void FilePrinter::closeFunction() {
  *os_ << ")";
}

/*----------------------------------------------------------------------------*/

void FilePrinter::openSection(char separator) {
  *os_ << separator << "\n";
  depth_++;
}

/*----------------------------------------------------------------------------*/

void FilePrinter::closeSection(char separator) {
  depth_--;
  indent();
  *os_ << separator;
}

/*----------------------------------------------------------------------------*/

void FilePrinter::indent(size_t extra_depth) {
  std::fill_n(std::ostreambuf_iterator<char>(*os_),
      2 * (depth_ + extra_depth), ' ');
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
}  // namespace tops
