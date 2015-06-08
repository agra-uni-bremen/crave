#include "ConstraintManager.hpp"

namespace crave {

    ConstraintManager::ConstraintManager() : changed_(false) {
        static unsigned ID = 0;
        id_ = ++ID;
    }

    bool ConstraintManager::enableConstraint(std::string const& key) {
        LOG(INFO) << "Enable constraint " << key << " in set " << id_ << ": ";
        ConstraintMap::iterator ite = constr_map_.find(key);
        if (ite != constr_map_.end()) {
            if (!ite->second->isEnabled()) {
                LOG(INFO) << "  ok";
                ite->second->enable();
                changed_ = true;
            } else {
                LOG(INFO) << "  already enabled";
            }
            return true;
        }
        LOG(INFO) << "not found";
        return false;
    }

    bool ConstraintManager::disableConstraint(std::string const& key) {
        LOG(INFO) << "Disable constraint " << key << " in set " << id_ << ": ";
        ConstraintMap::iterator ite = constr_map_.find(key);
        if (ite != constr_map_.end()) {
            if (ite->second->isEnabled()) {
                LOG(INFO) << "  ok";
                ite->second->disable();
                changed_ = true;
            } else {
                LOG(INFO) << "  already enabled";
            }
            return true;
        }
        LOG(INFO) << "  not found";
        return false;
    }
    
    bool ConstraintManager::isConstraintEnabled(std::string const& key) {
    ConstraintMap::iterator ite = constr_map_.find(key);
    return ite != constr_map_.end() && ite->second->isEnabled();
  }
    
    bool ConstraintManager::isChanged() const { return changed_; }
    void ConstraintManager::resetChanged() { changed_ = false; }
    
    std::ostream& ConstraintManager::printDotGraph(std::ostream& os) {
    ToDotVisitor visitor(os);

    BOOST_FOREACH(ConstraintPtr c, constraints_) {
      int32 a = reinterpret_cast<int32>(&*c);
      int32 b = reinterpret_cast<int32>(&(*c->expr()));
      os << "\t" << a << " [label=\"" << c->name()
         << (c->isSoft() ? " soft" : "") << (c->isCover() ? " cover" : "")
         << (!c->isEnabled() ? " disabled" : "") << "\"]" << std::endl;
      os << "\t" << a << " -> " << b << std::endl;
      c->expr()->visit(visitor);
    }
    return os;
  }

}
