#include "../crave/ir/UserConstraint.hpp"

namespace crave {

    unsigned UserConstraint::id() const {
        return id_;
    }

    expression const& UserConstraint::expr() const {
        return expr_;
    }

    std::string UserConstraint::name() const {
        return name_;
    }

    bool UserConstraint::isSoft() const {
        return soft_;
    }

    bool UserConstraint::isCover() const {
        return cover_;
    }

    bool UserConstraint::isEnabled() const {
        return enabled_;
    }

    void UserConstraint::enable() {
        enabled_ = true;
    }

    void UserConstraint::disable() {
        enabled_ = false;
    }

    bool UserConstraint::isVectorConstraint() {
        return false;
    }

    bool UserVectorConstraint::isUnique() {
        return unique_;
    }

    int UserVectorConstraint::getVectorId() {
        return *support_vars_.begin();
    }

    bool UserVectorConstraint::isVectorConstraint() {
        return true;
    }

    iterator ConstraintPartition::begin() {
        return constraints_.begin();
    }

    const_iterator ConstraintPartition::begin() const {
        return constraints_.begin();
    }

    iterator ConstraintPartition::end() {
        return constraints_.end();
    }

    const_iterator ConstraintPartition::end() const {
        return constraints_.end();
    }

    void ConstraintPartition::add(ConstraintPtr c) {
        iterator ite = constraints_.begin();
        while (ite != constraints_.end() && ((*ite)->id() > c->id())) ite++;
        constraints_.insert(ite, c);
    }

    bool ConstraintPartition::containsVar(int id) const {
        return support_vars_.find(id) != support_vars_.end();
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
            Context *ctx, bool const soft = false,
            bool const cover = false) {
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
                !c->isCover()); // soft cover constraint not defined/supported yet
        assert(!c->isVectorConstraint() ||
                !c->isCover()); // cover vector constraint not defined/supported yet

        changed_ = true;
        constraints_.push_back(c);

        return constr_map_[name] = c;
    }

    std::ostream& ConstraintManager::printDotGraph(std::ostream& os) {
        ToDotVisitor visitor(os);

        BOOST_FOREACH(ConstraintPtr c, constraints_) {
            long a = reinterpret_cast<long> (&*c);
            long b = reinterpret_cast<long> (&(*c->expr()));
            os << "\t" << a << " [label=\"" << c->name()
                    << (c->isSoft() ? " soft" : "") << (c->isCover() ? " cover" : "")
                    << (!c->isEnabled() ? " disabled" : "") << "\"]" << std::endl;
            os << "\t" << a << " -> " << b << std::endl;
            c->expr()->visit(&visitor);
        }
        return os;
    }

    void ConstraintPartitioner::reset() {
        constr_mngs_.clear();
        constraints_.clear();
        partitions_.clear();
        vec_constraints_.clear();
    }

    void ConstraintPartitioner::mergeConstraints(const ConstraintManager& mng) {
        LOG(INFO) << "Merge set " << mng.id_ << " with set(s)";

        BOOST_FOREACH(unsigned id, constr_mngs_) {
            LOG(INFO) << " " << id;
        }
        constr_mngs_.insert(mng.id_);

        BOOST_FOREACH(ConstraintPtr c, mng.constraints_) {
            if (c->isEnabled()) {
                if (c->isVectorConstraint())
                    vec_constraints_.push_back(
                        boost::static_pointer_cast<UserVectorConstraint>(c));

                else
                    constraints_.push_back(c);
            }
        }
    }

    void ConstraintPartitioner::partition() {
        while (!constraints_.empty()) {
            ConstraintPartition cp;
            maximizePartition(&cp, &constraints_);
            partitions_.push_back(cp);
        }
        LOG(INFO) << "Partition results of set(s)";

        BOOST_FOREACH(unsigned id, constr_mngs_) {
            LOG(INFO) << " " << id;
        }
        LOG(INFO) << ": ";

        LOG(INFO) << "  " << vec_constraints_.size() << " vector constraint(s):";

        BOOST_FOREACH(VectorConstraintPtr c, vec_constraints_) {
            LOG(INFO) << "   " << c->name();
        }

        LOG(INFO) << "  " << partitions_.size() << " constraint partition(s):";
        uint cnt = 0;

        BOOST_FOREACH(ConstraintPartition& cp, partitions_) {
            LOG(INFO) << "    #" << ++cnt << ": " << cp;
        }
    }

    std::vector<ConstraintPartition>& ConstraintPartitioner::getPartitions() {
        return partitions_;
    }

    std::vector<VectorConstraintPtr>& ConstraintPartitioner::getVectorConstraints() {
        return vec_constraints_;
    }
    
    void ConstraintPartitioner::maximizePartition(ConstraintPartition *cp, ConstraintList *lc) {
            ConstraintPtr c = lc->front();
            lc->pop_front();
            cp->support_vars_ = c->support_vars_;
            cp->add(c);
            while (true) {
                bool changed = false;
                ConstraintList::iterator ite = lc->begin();

                while (ite != lc->end()) {
                    c = *ite;
                    std::vector<int> v_intersection;
                    std::set_intersection(cp->support_vars_.begin(),
                            cp->support_vars_.end(),
                            c->support_vars_.begin(), c->support_vars_.end(),
                            std::back_inserter(v_intersection));
                    if (!v_intersection.empty()) {
                        changed = true;
                        cp->add(c);
                        cp->support_vars_.insert(c->support_vars_.begin(),
                                c->support_vars_.end());
                        ite = lc->erase(ite);
                    } else {
                        ++ite;
                    }
                }
                if (!changed) break;
            }
        }
}