#include "ContextTree.hpp"

namespace tops {
  namespace model {
      ContextTree::ContextTree(int alphabet_size) {
        auto alphabet = tops::AlphabetPtr(new Alphabet());
        for (int i = 0; i < alphabet_size; i++) {
          alphabet->createSymbol(std::to_string(i));
        }
        _self = tops::ContextTreePtr(new tops::ContextTree(alphabet));
      }

      ContextTreeNodePtr ContextTree::root() const {
        auto child = ContextTreeNodePtr(new ContextTreeNode(DiscreteIIDModelPtr(new DiscreteIIDModel({0}))));
        child->_self = _self->getRoot();
        return child;
      }

      ContextTreeNodePtr ContextTree::createContext(DiscreteIIDModelPtr distribution) {
        auto child = ContextTreeNodePtr(new ContextTreeNode(DiscreteIIDModelPtr(new DiscreteIIDModel({0}))));
        child->_self = _self->createContext();
        child->distribution(distribution);
        return child;
      }

      ContextTreeNodePtr ContextTree::context (int context_id) const {
        auto child = ContextTreeNodePtr(new ContextTreeNode(DiscreteIIDModelPtr(new DiscreteIIDModel({0}))));
        child->_self = _self->getContext(context_id);
        return child;
      }

      ContextTreeNodePtr ContextTree::context(const Sequence & s, int index) const {
        auto child = ContextTreeNodePtr(new ContextTreeNode(DiscreteIIDModelPtr(new DiscreteIIDModel({0}))));
        child->_self = _self->getContext(s, index);
        return child;
      }
  }
}