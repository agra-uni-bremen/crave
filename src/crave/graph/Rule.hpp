#pragma once

#include <boost/function.hpp>
#include <boost/proto/proto.hpp>
#include <boost/shared_ptr.hpp>

namespace crave {

namespace graph {

namespace proto = boost::proto;

#define NAMED_RULE(name) crave::graph::rule_type name = { crave::graph::Rule(#name) };

struct Rule;
typedef boost::shared_ptr<Rule> RulePtr;
typedef proto::terminal<Rule>::type rule_type;

struct Rule {
  boost::function<void()> entry;
  boost::function<void()> exit;

  Rule(const char* name) : m_name(name), entry(0), exit(0) { }

  const char* name() const { return m_name; }

private:
  const char* m_name;
};

};

};

