#include "../crave/ir/ConstraintManager.hpp"


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

    bool ConstraintManager::isChanged() const {
        return changed_;
    }

    void ConstraintManager::resetChanged() {
        changed_ = false;
    }
    
    ConstraintPtr ConstraintManager::makeConstraint(std::string const& name, int c_id, NodePtr n,
                               Context *ctx, bool const soft,
                               bool const cover) {
    LOG(INFO) << "New " << (soft ? "soft " : "") << (cover ? "cover " : "")
              << "constraint " << name << " in set " << id_;

    if (constr_map_.find(name) != constr_map_.end()) {
      throw std::runtime_error(name + "Constraint already exists.");
    }

    GetSupportSetVisitor gssv;
    n->visit(&gssv);

    ConstraintPtr c(
        boost::dynamic_pointer_cast<ForEach>(n) != 0
            ? new UserVectorConstraint(c_id, n, name, gssv.getSupportVars(),
                                       false, soft, cover)
            : (boost::dynamic_pointer_cast<Unique>(n) != 0
                   ? new UserVectorConstraint(c_id, n, name,
                                              gssv.getSupportVars(), true, soft,
                                              cover)
                   : new UserConstraint(c_id, n, name, gssv.getSupportVars(),
                                        soft, cover)));

    assert(!c->isSoft() ||
           !c->isCover());  // soft cover constraint not defined/supported yet
    assert(!c->isVectorConstraint() ||
           !c->isCover());  // cover vector constraint not defined/supported yet

    changed_ = true;
    constraints_.push_back(c);

    return constr_map_[name] = c;
  }
    
    std::ostream& ConstraintManager::printDotGraph(std::ostream& os) {
    ToDotVisitor visitor(os);
    BOOST_FOREACH(ConstraintPtr c, constraints_) {
      long a = reinterpret_cast<long>(&*c);
      long b = reinterpret_cast<long>(&(*c->expr()));
      os << "\t" << a << " [label=\"" << c->name()
         << (c->isSoft() ? " soft" : "") << (c->isCover() ? " cover" : "")
         << (!c->isEnabled() ? " disabled" : "") << "\"]" << std::endl;
      os << "\t" << a << " -> " << b << std::endl;
      c->expr()->visit(&visitor);
    }
    return os;
  }
}