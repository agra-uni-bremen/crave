#pragma once

#include <iostream>
#include <string>
#include <set>
#include <functional>

#include <boost/proto/proto.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/assert.hpp>

#include "graph/Node.hpp"

#include "../ConstrainedRandom.hpp"

#define NAMED_RULE(name) \
  crave::graph::rule_type name = {crave::graph::Rule(#name)};

namespace crave {

namespace graph {

namespace proto = boost::proto;

struct Rule;
typedef boost::shared_ptr<Rule> RulePtr;
typedef proto::terminal<Rule>::type rule_type;
typedef std::map<std::string, Rule*> rule_map;
typedef std::function<void()> action_type;

extern rule_map global_rule_map;

struct Rule {
  action_type entry;
  action_type main;
  action_type exit;

  Rule(const char* name)
      : entry(), main(), exit(), m_name(name), m_rand_obj(0) {
    global_rule_map[name] = this;
  }

  const char* name() const { return m_name; }

  void bind_rand_obj(rand_obj* obj) {
    m_rand_obj = obj;
    main = boost::bind(&Rule::gen, this);
  }

  bool is_rand_obj_covered() const {
    return m_rand_obj == 0 || m_rand_obj->constraint_.isCovered();
  }
  void reset_coverage() {
    if (m_rand_obj) m_rand_obj->constraint_.resetCoverage();
  }

 private:
  const char* m_name;
  rand_obj* m_rand_obj;

  void gen() {
    if (m_rand_obj) {
      if (is_rand_obj_covered())
        LOG(INFO)
            << "Rule " << m_name
            << " has its rand_obj covered, now generate a random solution";
      assert(m_rand_obj->next_cov());
    }
  }
};

struct RuleContext : proto::callable_context<RuleContext, proto::null_context> {
  typedef NodePtr result_type;

  template <typename Expr>
  RuleContext& operator()(Expr expr) {
    BOOST_ASSERT(proto::eval(expr, *this));
    return *this;
  }

  Rule& operator[](rule_type& r) { return proto::value(r); }

  result_type operator()(proto::tag::terminal, Rule& r) {
    if (m_named_nodes.find(r.name()) != m_named_nodes.end())
      return m_named_nodes[r.name()];
    return m_named_nodes[r.name()] = NodePtr(new Terminal(r.name()));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(proto::tag::shift_right, Expr1& e1, Expr2& e2) {
    Sequence* seq = new Sequence;
    seq->add(proto::eval(e1, *this));
    seq->add(proto::eval(e2, *this));
    return NodePtr(seq);
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(proto::tag::bitwise_or, Expr1& e1, Expr2& e2) {
    Selector* sel = new Selector;
    sel->add(proto::eval(e1, *this));
    sel->add(proto::eval(e2, *this));
    return NodePtr(sel);
  }

  template <typename Expr>
  result_type operator()(proto::tag::assign, rule_type& r, Expr& e) {
    NodePtr n = proto::eval(e, *this);
    BOOST_ASSERT_MSG(n->name() == 0, "RHS must be unnamed");
    const char* rule_name = proto::value(r).name();
    BOOST_ASSERT_MSG(m_assigned_nodes.find(rule_name) == m_assigned_nodes.end(),
                     "Multiple assignments to a rule is not allowed");
    n->rename(rule_name);
    m_assigned_nodes.insert(n->name());
    return m_named_nodes[n->name()] = n;
  }

  void root(rule_type& r);
  void print_dot_graph(rule_type& r, std::ostream& out);
  void to_dot_file(rule_type& r, const char* filename);
  void display_graph(rule_type& r);

 private:
  node_map m_named_nodes;
  std::set<std::string> m_assigned_nodes;
  NodePtr m_root;
};
};
};
