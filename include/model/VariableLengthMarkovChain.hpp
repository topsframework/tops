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

#ifndef VARIABLE_LENGTH_MARKOV_CHAIN_HPP_
#define VARIABLE_LENGTH_MARKOV_CHAIN_HPP_

#include <memory>

#include "FactorableModel.hpp"
#include "ContextTree.hpp"

namespace tops {
namespace model {

class VariableLengthMarkovChain : public FactorableModel {
 public:
  VariableLengthMarkovChain(ContextTreePtr tree, int alphabetSize);

  virtual int alphabetSize();

  virtual double evaluate(const Sequence &sequence,
                          unsigned int begin,
                          unsigned int end);
  virtual double evaluatePosition(const Sequence &sequence,
                                  unsigned int index) const;

  virtual double choose() const;
  virtual Symbol choosePosition(const Sequence &sequence,
                                unsigned int index) const;
};
typedef std::shared_ptr<VariableLengthMarkovChain> VariableLengthMarkovChainPtr;

}  // namespace model
}  // namespace tops

#endif
