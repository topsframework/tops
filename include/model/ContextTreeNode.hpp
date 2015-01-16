#ifndef CONTEXT_TREE_NODE_HPP_
#define CONTEXT_TREE_NODE_HPP_

#include <memory>

#include "DiscreteIIDModel.hpp"

#include "src/ContextTree.hpp"

namespace tops {
  namespace model {
    class ContextTreeNode;
    typedef std::shared_ptr<ContextTreeNode> ContextTreeNodePtr;

    class ContextTreeNode {
    public:
      ContextTreeNode(DiscreteIIDModelPtr distribution);

      DiscreteIIDModelPtr distribution(DiscreteIIDModelPtr distribution);
      DiscreteIIDModelPtr distribution();
      
      ContextTreeNodePtr child(ContextTreeNodePtr child, Symbol symbol);
      ContextTreeNodePtr child(Symbol symbol);

      // After refactoring, remove _self!
      tops::ContextTreeNodePtr _self;
    };
  }
}

#endif
