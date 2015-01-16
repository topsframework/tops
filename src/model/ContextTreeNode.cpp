#include "ContextTreeNode.hpp"

namespace tops {
  namespace model {
    ContextTreeNode::ContextTreeNode(DiscreteIIDModelPtr distribution) : _distribution(distribution) {
      _children.resize(distribution->alphabetSize());
    }

    DiscreteIIDModelPtr ContextTreeNode::distribution() {
      return _distribution;
    }

    ContextTreeNodePtr ContextTreeNode::child(ContextTreeNodePtr child, Symbol symbol) {
      _children[symbol] = child;
      return child;
    }

    ContextTreeNodePtr ContextTreeNode::child(Symbol symbol) {
      return _children[symbol];
    }
  }
}