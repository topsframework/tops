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

#ifndef TOPS_MODEL_EVALUATOR_
#define TOPS_MODEL_EVALUATOR_

namespace tops {
namespace model {

class Evaluator;

/**
 * @typedef EvaluatorPtr
 * @brief Alias of pointer to Evaluator.
 */
using EvaluatorPtr = std::shared_ptr<Evaluator>;

/**
 * @class Evaluator
 * @brief TODO
 */
class Evaluator : public std::enable_shared_from_this<Evaluator> {
 public:
  // Concrete methods
  virtual double probabilityOf(unsigned int begin,
                               unsigned int end,
                               unsigned int phase = 0) = 0;
};

}  // namespace model
}  // namespace tops

#endif