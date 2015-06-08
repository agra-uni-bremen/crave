// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <glog/logging.h>

#include <ostream>
#include <string>
#include <vector>
#include <list>
#include <stdexcept>
#include <map>
#include <set>

#include "Context.hpp"
#include "Node.hpp"
#include "visitor/FixWidthVisitor.hpp"
#include "visitor/GetSupportSetVisitor.hpp"
#include "visitor/ToDotNodeVisitor.hpp"

namespace crave {

    int new_constraint_id();

    struct ConstraintManager;
    struct ConstraintPartitioner;

    struct UserConstraint;
    typedef boost::shared_ptr<UserConstraint> ConstraintPtr;
    typedef std::list<ConstraintPtr> ConstraintList;

    struct UserVectorConstraint;
    typedef boost::shared_ptr<UserVectorConstraint> VectorConstraintPtr;

    /**
     *
     */
    struct UserConstraint {
        friend struct ConstraintManager;
        friend struct ConstraintPartitioner;

        typedef NodePtr expression;

    protected:

        UserConstraint(unsigned const id, expression const expr,
                std::string const& name, std::set<int> support_vars,
                bool const soft = false, bool const cover = false,
                bool const enabled = true)
        : id_(id),
        expr_(expr),
        name_(name),
        support_vars_(support_vars),
        soft_(soft),
        cover_(cover),
        enabled_(enabled) {
        }

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

        bool isSoft();

        bool isCover();

        bool isEnabled();

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

    /**
     *
     */
    struct UserVectorConstraint : UserConstraint {
        friend struct ConstraintManager;
        friend struct ConstraintPartitioner;

    protected:

        UserVectorConstraint(unsigned const id, expression const expr,
                std::string const& name, std::set<int> support_vars,
                bool const unique, bool const soft = false,
                bool const cover = false, bool const enabled = true)
        : UserConstraint(id, expr, name, support_vars, soft, cover, enabled),
        unique_(unique) {
        }

    public:
        bool isUnique();
        int getVectorId();
        bool isVectorConstraint();

    protected:
        bool unique_;
    };

    /**
     *
     */
    struct ConstraintPartition {
        friend struct ConstraintPartitioner;

        ConstraintPartition() {
        }

        typedef ConstraintList::iterator iterator;
        typedef ConstraintList::const_iterator const_iterator;
        typedef ConstraintList::reference reference;
        typedef ConstraintList::const_reference const_reference;
        typedef ConstraintList::size_type size_type;
        typedef ConstraintList::value_type value_type;

        iterator begin();

        const_iterator begin() const;

        iterator end();

        const_iterator end() const;
        void add(ConstraintPtr c);

        bool containsVar(int id) const;

        template <typename ostream>
        friend ostream& operator<<(ostream& os, const ConstraintPartition& cp) {
            os << "[ ";

            BOOST_FOREACH(ConstraintPtr c, cp) {
                os << c->name() << " ";
            }
            os << "]";
            os << std::flush;
            return os;
        }

    private:
        ConstraintList constraints_;
        std::set<int> support_vars_;
    };

    /**
     *
     */
    struct ConstraintManager {
        friend struct ConstraintPartitioner;

        typedef std::map<std::string, ConstraintPtr> ConstraintMap;

        ConstraintManager() : changed_(false) {
            static unsigned ID = 0;
            id_ = ++ID;
        }

        template <typename ostream>
        friend ostream& operator<<(ostream& os, const ConstraintManager& set) {
            os << "Set " << set.id_ << " has " << set.constraints_.size()
                    << " constraint(s) and has " << (set.changed_ ? "" : "not ") << "changed"
                    << std::endl;

            BOOST_FOREACH(ConstraintPtr item, set.constraints_) {
                os << item << std::endl;
            }
            os << std::flush;
            return os;
        }

        bool enableConstraint(std::string const& key);

        bool disableConstraint(std::string const& key);

        bool isConstraintEnabled(std::string const& key);

        bool isChanged() const;

        void resetChanged();

        template <typename Expr>
        ConstraintPtr makeConstraint(std::string const& name, int c_id, Expr e,
                Context *ctx, bool const soft = false,
                bool const cover = false);

        template <typename Expr>
        ConstraintPtr makeConstraint(std::string const& name, Expr e, Context *ctx,
                bool const soft = false,
                bool const cover = false) {
            return makeConstraint(name, new_constraint_id(), e, ctx, soft, cover);
        }

        template <typename Expr>
        ConstraintPtr makeConstraint(Expr e, Context *ctx, bool const soft = false,
                bool const cover = false) {
            int id = new_constraint_id();
            return makeConstraint("constraint_" + boost::lexical_cast<std::string>(id),
                    id, e, ctx, soft, cover);
        }

        std::ostream& printDotGraph(std::ostream& os);

    private:
        unsigned id_;
        ConstraintMap constr_map_;
        ConstraintList constraints_;
        bool changed_;
    };

    /**
     *
     */
    struct ConstraintPartitioner {
        void reset();

        void mergeConstraints(const ConstraintManager& mng);

        void partition();

        std::vector<ConstraintPartition>& getPartitions();

        std::vector<VectorConstraintPtr>& getVectorConstraints();

    private:

        void maximizePartition(ConstraintPartition *cp, ConstraintList *lc);

    private:
        std::set<unsigned> constr_mngs_;
        ConstraintList constraints_;

        // results
        std::vector<ConstraintPartition> partitions_;
        std::vector<VectorConstraintPtr> vec_constraints_;
    };

} // end namespace crave
