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

#ifndef TOPS_MODEL_CONTEXT_TREE_
#define TOPS_MODEL_CONTEXT_TREE_

#include <memory>
#include <vector>
#include <set>

#include "model/ContextTreeNode.hpp"
#include "model/Sequence.hpp"

namespace tops {
namespace model {

class ContextTree;
typedef std::shared_ptr<ContextTree> ContextTreePtr;

class ContextTree {
 public:
  static ContextTreePtr make(int alphabet_size);

  int alphabetSize() const;
  std::vector<ContextTreeNodePtr> & all_context();
  ContextTreeNodePtr getRoot() const;
  ContextTreeNodePtr createContext();
  ContextTreeNodePtr getContext (int id);
  ContextTreeNodePtr getContext(const Sequence & s, int i);
  std::set<int> getLevelOneNodes();
  void removeContextNotUsed();
  void normalize();
  void normalize(ProbabilisticModelPtr old, double pseudocount, int i);
  void initializeCounter(const std::vector<Sequence> &sequences, int order, const std::map<std::string, double> &weights);
  void initializeCounter(const std::vector<Sequence> &sequences, int order, double pseudocounts, const std::map<std::string, double> & weights);
  void pruneTree(double delta);
  void pruneTreeSmallSampleSize(int small_);
  void initializeContextTreeRissanen(const std::vector<Sequence> &sequences);
  int getNumberOfNodes() const;

 private:
  std::vector<ContextTreeNodePtr> _all_context;
  int _alphabet_size;

  explicit ContextTree(int alphabet_size);

  void printTree(ContextTreeNodePtr node, std::stringstream & out) const;
  void buildParameters(ContextTreeNodePtr node, std::map<std::string, double> & parameters) const;

};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_CONTEXT_TREE_
