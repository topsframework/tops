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

#ifndef TOPS_APP_MYOP_
#define TOPS_APP_MYOP_

// Standard headers
#include <string>

// Internal headers
#include "model/Sequence.hpp"

#include "model/GeneralizedHiddenMarkovModel.hpp"

namespace tops {
namespace myop {

class Myop {
 public:
  // Constructors
  Myop(std::string dataset);

  // Concrete methods
  model::Sequence predict(const model::Sequence& observation);

 private:
  // Static variables
  static const std::size_t alphabetSize;

  // Instance variables
  model::GeneralizedHiddenMarkovModelPtr _ghmm;
};

}  // namespace myop
}  // namespace tops

#endif  // TOPS_APP_MYOP_
