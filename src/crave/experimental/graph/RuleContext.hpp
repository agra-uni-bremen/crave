// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.
#pragma once

#include <boost/proto/proto.hpp>
#include "Rule.hpp"

namespace crave {

namespace graph {

namespace proto = boost::proto;
    
typedef std::shared_ptr<Rule> RulePtr;
typedef proto::terminal<Rule>::type rule_type;

/**
 * Graph-based RuleContext, highly experimental
 */
struct RuleContext : proto::callable_context<RuleContext, proto::null_context> {
  typedef NodePtr result_type;

  /**
   * constructor
   * @param the first rule
   */
  template <typename Expr>
  RuleContext& operator()(Expr expr) {
    BOOST_ASSERT(proto::eval(expr, *this));
    return *this;
  }

  /**
   * access operator
   * @param r proto::terminal<Rule>
   * @return Rule
   */
  Rule& operator[](rule_type& r) { return proto::value(r); }

  /**
   * Terminal rule
   */
  result_type operator()(proto::tag::terminal, Rule& r) {
    if (m_named_nodes.find(r.name()) != m_named_nodes.end()) return m_named_nodes[r.name()];
    return m_named_nodes[r.name()] = NodePtr(new Terminal(r.name()));
  }

  /**
   * e1 >> e2
   */
  template <typename Expr1, typename Expr2>
  result_type operator()(proto::tag::shift_right, Expr1& e1, Expr2& e2) {
    Sequence* seq = new Sequence;
    seq->add(proto::eval(e1, *this));
    seq->add(proto::eval(e2, *this));
    return NodePtr(seq);
  }

  /**
   * e1 || e2
   */
  template <typename Expr1, typename Expr2>
  result_type operator()(proto::tag::bitwise_or, Expr1& e1, Expr2& e2) {
    Selector* sel = new Selector;
    sel->add(proto::eval(e1, *this));
    sel->add(proto::eval(e2, *this));
    return NodePtr(sel);
  }

  /**
   * Rule assignment (r = e)
   */
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

  /**
   * Set the root rule
   * @param r rule
   */
  void root(rule_type& r);

  /**
   * print dot graph to an output stream
   * @param r the root rule
   * @param out output stream
   */
  void print_dot_graph(rule_type& r, std::ostream& out);

  /**
   * print to dot graph to file
   * @param r the root rule
   * @param filename output file
   */
  void to_dot_file(rule_type& r, const char* filename);

  /**
   * display graph
   * @param r the root rule
   */
  void display_graph(rule_type& r);

 private:
  node_map m_named_nodes;
  std::set<std::string> m_assigned_nodes;
  NodePtr m_root;
};
}
}
