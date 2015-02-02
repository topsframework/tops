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

#ifndef TOPS_MODEL_INHOMOGENEOUS_MARKOV_CHAIN_
#define TOPS_MODEL_INHOMOGENEOUS_MARKOV_CHAIN_

#include <memory>
#include <vector>

#include "VariableLengthMarkovChain.hpp"

namespace tops {
namespace model {

class InhomogeneousMarkovChain;
typedef std::shared_ptr<InhomogeneousMarkovChain> InhomogeneousMarkovChainPtr;

class InhomogeneousMarkovChain {
 public:
  static InhomogeneousMarkovChainPtr make(std::vector<VariableLengthMarkovChainPtr> vlmcs, bool phased);

  virtual int alphabetSize() const;
  virtual double evaluatePosition(const Sequence &s, unsigned int i) const;
  virtual int choosePosition(const Sequence &s, unsigned int i) const;
  virtual double evaluateSequence(const Sequence &s, unsigned int begin, unsigned int end) const;

 private:
  InhomogeneousMarkovChain(std::vector<VariableLengthMarkovChainPtr> vlmcs, bool phased);

  std::vector<VariableLengthMarkovChainPtr> _vlmcs;
  bool _phased;
};

}
}

#endif  // TOPS_MODEL_INHOMOGENEOUS_MARKOV_CHAIN_
