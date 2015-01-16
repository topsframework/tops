#include "ContextTreeNode.hpp"

namespace tops {
  namespace model {
    ContextTreeNode::ContextTreeNode(DiscreteIIDModelPtr distribution) {
      _self = tops::ContextTreeNodePtr(new tops::ContextTreeNode(distribution->alphabetSize()));
      this->distribution(distribution);
    }

    DiscreteIIDModelPtr ContextTreeNode::distribution(DiscreteIIDModelPtr distribution) {
      _self->setDistribution(boost::static_pointer_cast<tops::DiscreteIIDModel>(distribution->_self));
      return distribution;
    }

    DiscreteIIDModelPtr ContextTreeNode::distribution() {
      auto iid = DiscreteIIDModelPtr(new DiscreteIIDModel({}));
      iid->_self = _self->getDistribution();
      return iid;
    }

    ContextTreeNodePtr ContextTreeNode::child(ContextTreeNodePtr child, Symbol symbol) {
      _self->setChild(child->_self, symbol);
      return child;
    }

    ContextTreeNodePtr ContextTreeNode::child(Symbol symbol) {
      auto child = ContextTreeNodePtr(new ContextTreeNode(DiscreteIIDModelPtr(new DiscreteIIDModel({0}))));
      child->_self = _self->getChild(symbol);
      return child;
    }
  }
}