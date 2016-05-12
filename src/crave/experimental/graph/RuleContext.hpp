// Copyright 2014 The CRAVE developers. All rights reserved.
#pragma once

#include "Rule.hpp"
#include <boost/proto/proto.hpp>

namespace crave {

namespace graph {

namespace proto = boost::proto;
    
typedef boost::shared_ptr<Rule> RulePtr;
typedef proto::terminal<Rule>::type rule_type;

struct RuleContext : proto::callable_context<RuleContext, proto::null_context> {
  typedef NodePtr result_type;

  template <typename Expr>
  RuleContext& operator()(Expr expr) {
    BOOST_ASSERT(proto::eval(expr, *this));
    return *this;
  }

  Rule& operator[](rule_type& r) { return proto::value(r); }

  result_type operator()(proto::tag::terminal, Rule& r) {
    if (m_named_nodes.find(r.name()) != m_named_nodes.end()) return m_named_nodes[r.name()];
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
}
}