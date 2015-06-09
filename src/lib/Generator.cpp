#include "../crave/backend/Generator.hpp"

namespace crave {

    template <typename Expr>
  Generator::Generator(Expr expr)
      : constr_mng_(),
        var_ctn_(&crave::variables),
        ctx_(var_ctn_),
        var_gen_(var_ctn_),
        vec_gen_(var_gen_),
        var_cov_gen_(var_ctn_),
        vec_cov_gen_(var_cov_gen_),
        covered_(false) {
    (*this)(expr);
  }
    
    Generator::Generator()
    : constr_mng_(),
    var_ctn_(&crave::variables),
    ctx_(var_ctn_),
    var_gen_(var_ctn_),
    vec_gen_(var_gen_),
    var_cov_gen_(var_ctn_),
    vec_cov_gen_(var_cov_gen_),
    covered_(false) {
    }

    bool Generator::enableConstraint(std::string const& name) {
        return constr_mng_.enableConstraint(name);
    }

    bool Generator::disableConstraint(std::string const& name) {
        return constr_mng_.disableConstraint(name);
    }

    bool Generator::isConstraintEnabled(std::string const& name) {
        return constr_mng_.isConstraintEnabled(name);
    }

    bool Generator::isChanged() {
        return constr_mng_.isChanged();
    }

    Generator& Generator::operator()() {
        if (!next()) {
            throw std::runtime_error("Generator constraint unsatisfiable.");
        }
        return *this;
    }

    void Generator::merge(const Generator& other) {
        constr_pttn_.mergeConstraints(other.constr_mng_);
    }

    void Generator::reset() {
        constr_mng_.resetChanged();
        constr_pttn_.reset();
        resetCoverage();
    }

    void Generator::rebuild(bool selfInclude) {
        if (selfInclude) merge(*this);
        constr_pttn_.partition();
        // currently, every hard/soft/cover constraint is considered for
        // partitioning, this is suboptimal.
        var_gen_.reset(constr_pttn_.getPartitions());
        vec_gen_.reset(constr_pttn_.getVectorConstraints());
        var_cov_gen_.reset(constr_pttn_.getPartitions());
        vec_cov_gen_.reset(constr_pttn_.getVectorConstraints());
    }

    bool Generator::next() {
        if (constr_mng_.isChanged()) {
            reset();
            rebuild(true);
        }
        return var_gen_.solve() && vec_gen_.solve();
    }

    bool Generator::nextCov() {
        if (constr_mng_.isChanged()) {
            reset();
            rebuild(true);
        }
        if (!covered_) {
            if (var_cov_gen_.solve() && vec_cov_gen_.solve())
                return true;
            else
                covered_ = true;
        }
        return next();
    }

    bool Generator::isCovered() {
        return covered_;
    }

    void Generator::resetCoverage() {
        covered_ = false;
        var_cov_gen_.reset(constr_pttn_.getPartitions());
        vec_cov_gen_.reset(constr_pttn_.getVectorConstraints());
    }

    std::ostream& Generator::printDotGraph(std::ostream& os, bool root) {
        if (root) os << "digraph AST {" << std::endl;
        constr_mng_.printDotGraph(os);
        if (root) os << "}" << std::endl;
        return os;
    }

    std::vector<std::vector<std::string> > Generator::analyseContradiction() {
        return var_gen_.analyseContradiction();
    }

    std::vector<std::string> Generator::getInactiveSofts() {
        return var_gen_.getInactiveSofts();
    }
}
