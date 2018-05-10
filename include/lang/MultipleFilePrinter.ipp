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

// Internal headers
#include <memory>
#include <string>
#include <utility>

namespace tops {
namespace lang {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

template<typename... Args>
MultipleFilePrinter::MultipleFilePrinter(
    bool change_ostream, const std::string &root_dir, Args&&... args)
    : Base(std::forward<Args>(args)...),
      change_ostream_(change_ostream), root_dir_(root_dir) {
}

/*----------------------------------------------------------------------------*/
/*                               STATIC METHODS                               */
/*----------------------------------------------------------------------------*/

template<typename... Args>
decltype(auto) MultipleFilePrinter::make(Args&&... args) {
  return std::shared_ptr<Self>(new Self(std::forward<Args>(args)...));
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
}  // namespace tops
