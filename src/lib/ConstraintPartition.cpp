#include "../crave/ir/ConstraintPartition.hpp"
#include <ostream>

namespace crave {

    template <typename ostream>
    ostream& operator<<(ostream& os, const ConstraintPartition& cp) {
        os << "[ ";

        BOOST_FOREACH(ConstraintPtr c, cp) {
            os << c->name() << " ";
        }
        os << "]";
        os << std::flush;
        return os;
    }
    
    
    template std::ostream& operator<< <std::ostream>(std::ostream& os, const ConstraintPartition& cp);
    
    typedef ConstraintList::iterator iterator;
    typedef ConstraintList::const_iterator const_iterator;

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
}
