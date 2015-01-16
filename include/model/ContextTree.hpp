#ifndef CONTEXT_TREE_HPP_
#define CONTEXT_TREE_HPP_

#include <memory>

#include "ContextTreeNode.hpp"

#include "src/ContextTree.hpp"

namespace tops {
  namespace model {
    class ContextTree {
    public:
      ContextTree(int alphabet_size);

      ContextTreeNodePtr root() const;

      ContextTreeNodePtr createContext(DiscreteIIDModelPtr distribution);

      ContextTreeNodePtr context (int context_id) const;
      ContextTreeNodePtr context(const Sequence & s, int index) const;

      // After refactoring, remove _self!
      tops::ContextTreePtr _self;
    };

    typedef std::shared_ptr<ContextTree> ContextTreePtr;
  }
}

#endif
