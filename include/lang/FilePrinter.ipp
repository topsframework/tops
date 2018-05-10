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

// Standard header
#include <memory>
#include <string>
#include <utility>
#include <fstream>
#include <algorithm>

namespace tops {
namespace lang {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

template<typename... OptionArgs>
FilePrinter::FilePrinter(std::shared_ptr<std::ostream> os,
                         size_t initial_depth,
                         OptionArgs&&... option_args)
    : os_(os), depth_(initial_depth),
      option_{ std::forward<OptionArgs>(option_args)... } {
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename Return, typename... Params>
void FilePrinter::print(const std::function<Return(Params...)> &/* func */) {
  *os_ << "fun()";
}

/*----------------------------------------------------------------------------*/

template<typename Pair>
auto FilePrinter::print(const Pair &pair)
    -> std::enable_if_t<is_pair<Pair>::value> {
  print(pair.first);
  *os_ << ": ";
  print(pair.second);
}

/*----------------------------------------------------------------------------*/

template<typename Iterable>
auto FilePrinter::print(const Iterable &iterable)
    -> std::enable_if_t<is_iterable<Iterable>::value> {
  openSection('[');
  for (auto it = std::begin(iterable); it != std::end(iterable); ++it) {
    indent();
    print(*it);
    *os_ << (it == std::prev(std::end(iterable)) ? "" : ",") << "\n";
  }
  closeSection(']');
}

/*----------------------------------------------------------------------------*/

template<typename Base, typename... Options>
void FilePrinter::copy(
    std::shared_ptr<config::BasicConfig<Base, Options...>> config_ptr,
    std::shared_ptr<std::ostream> os) {
  std::ifstream src(config_ptr->path());
  std::string line;
  while (std::getline(src, line)) {
    indent();
    *os << line << std::endl;
  }
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
}  // namespace tops
