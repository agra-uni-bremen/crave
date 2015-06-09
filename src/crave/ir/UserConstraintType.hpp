// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <boost/foreach.hpp>
#include <set>
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

#include "Node.hpp"

namespace crave {

    

    struct UserConstraint {
        friend struct ConstraintManager;
        friend struct ConstraintPartitioner;

        typedef NodePtr expression;

    protected:
        UserConstraint(unsigned const id, expression const expr,
                std::string const& name, std::set<int> support_vars,
                bool const soft = false, bool const cover = false,
                bool const enabled = true);

    public:

        virtual ~UserConstraint() {
        }

        template <typename ostream>
        friend ostream& operator<<(ostream& os, const UserConstraint& constr) {
            os << constr.name_ << " is a " << (constr.soft_ ? "soft" : "hard")
                    << " constraint and " << (constr.enabled_ ? "enabled" : "disabled");
            os << ", support vars =";

            BOOST_FOREACH(int item, constr.support_vars_) {
                os << " " << item;
            }
            return os;
        }

        unsigned id() const;

        expression const& expr() const;

        std::string name() const;

        bool isSoft() const;

        bool isCover() const;

        bool isEnabled() const;

        void enable();

        void disable();

        virtual bool isVectorConstraint();

    protected:
        unsigned id_;
        expression expr_;
        std::string name_;
        std::set<int> support_vars_;
        bool soft_;
        bool cover_;
        bool enabled_;
    };
    
    typedef boost::shared_ptr<UserConstraint> ConstraintPtr;
    typedef std::list<ConstraintPtr> ConstraintList;
} // namespace crave
