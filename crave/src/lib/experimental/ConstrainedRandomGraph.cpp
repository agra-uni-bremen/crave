/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2012-2020 University of Bremen, Germany. 
//  	Copyright (c) 2015-2020 DFKI GmbH Bremen, Germany.
//  	Copyright (c) 2020 Johannes Kepler University Linz, Austria.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
****************************************************************************************/


#include <fstream>

#include <boost/assert.hpp>

#include "../../crave/experimental/ConstrainedRandomGraph.hpp"
#include "../../crave/experimental/graph/GraphVisitor.hpp"
#include "../../crave/utils/Logging.hpp"

namespace crave {

namespace graph {

rule_map global_rule_map;

void Terminal::accept(NodeVisitor& v) { v.visitTerminal(*this); }

void Selector::accept(NodeVisitor& v) { v.visitSelector(*this); }

void Sequence::accept(NodeVisitor& v) { v.visitSequence(*this); }

struct Executor : NodeVisitor {
  Executor(NodePtr r) : m_root(r), m_rules(global_rule_map), m_id(0), m_path_count(0) {}

  virtual void visitTerminal(Terminal&);
  virtual void visitSelector(Selector&);
  virtual void visitSequence(Sequence&);

 private:
  typedef std::pair<int, int> result_type;

  Rule* getRule(Node& n) {
    if (n.name() && m_rules.find(n.name()) != m_rules.end()) {
      return m_rules[n.name()];
    }
    return NULL;
  }

  void make_edge(int s, int d) { m_adj[s].push_back(d); }

  void check_root(Node&);
  void dfs(int);

  NodePtr m_root;
  rule_map& m_rules;
  int m_id;
  std::stack<result_type> m_stack;
  std::map<int, std::vector<int> > m_adj;
  std::map<int, action_type> m_actions;
  std::map<int, Rule*> m_main_to_rule_map;
  std::vector<int> path;
  int m_path_count;
};

void Executor::check_root(Node& n) {
  if (&n != m_root.get()) return;
  dfs(0);
}

void Executor::dfs(int v) {
  path.push_back(v);
  if (m_adj.find(v) == m_adj.end()) {
    BOOST_ASSERT_MSG(v == 2, "Invalid end of unfolded sequence");
    m_path_count++;
    // reset coverage of all rand_objs on the new path
    for(int i : path) {
      if (m_main_to_rule_map.find(i) != m_main_to_rule_map.end()) m_main_to_rule_map[i]->reset_coverage();
    }
    int iter_count = 0;
    while (true) {  // repeat until path is covered
      iter_count++;
      for(int i : path) {
        if (m_actions.find(i) != m_actions.end() && m_actions[i]) m_actions[i]();
      }
      bool path_covered = true;
      for(int i : path) {
        if (m_main_to_rule_map.find(i) != m_main_to_rule_map.end() && !m_main_to_rule_map[i]->is_rand_obj_covered()) {
          path_covered = false;
          break;
        }
      }
      if (path_covered) break;
    }
    LOG(INFO) << "Path " << m_path_count << " is covered after " << iter_count << " iteration(s)";
  } else {
    std::vector<int>& adj = m_adj[v];
    for(int i : adj) { dfs(i); }
  }
  path.pop_back();
}

void Executor::visitTerminal(Terminal& t) {
  Rule* r = getRule(t);
  BOOST_ASSERT_MSG(r, "A named rule could not be found");
  m_actions.insert(std::make_pair(m_id, r->entry));
  m_actions.insert(std::make_pair(m_id + 1, r->main));
  m_actions.insert(std::make_pair(m_id + 2, r->exit));
  m_main_to_rule_map.insert(std::make_pair(m_id + 1, r));
  make_edge(m_id, m_id + 1);
  make_edge(m_id + 1, m_id + 2);
  m_stack.push(result_type(m_id, m_id + 2));
  m_id += 3;
  check_root(t);
}

void Executor::visitSelector(Selector& nt) {
  int start = m_id;
  int end = m_id + 2;
  m_id += 3;
  m_stack.push(result_type(start, end));

  Rule* r = getRule(nt);
  if (r) {
    m_actions.insert(std::make_pair(start, r->entry));
    m_actions.insert(std::make_pair(start + 1, r->main));
    m_actions.insert(std::make_pair(end, r->exit));
    m_main_to_rule_map.insert(std::make_pair(start + 1, r));
  }

  make_edge(start, start + 1);

  for(NodePtr n : nt.children) {
    n->accept(*this);
    result_type& r = m_stack.top();
    m_stack.pop();
    make_edge(start + 1, r.first);
    make_edge(r.second, end);
  }

  check_root(nt);
}

void Executor::visitSequence(Sequence& nt) {
  int start = m_id;
  int end = m_id + 2;
  m_id += 3;
  m_stack.push(result_type(start, end));

  Rule* r = getRule(nt);
  if (r) {
    m_actions.insert(std::make_pair(start, r->entry));
    m_actions.insert(std::make_pair(start + 1, r->main));
    m_actions.insert(std::make_pair(end, r->exit));
    m_main_to_rule_map.insert(std::make_pair(start + 1, r));
  }

  make_edge(start, start + 1);

  int last = start + 1;
  for(NodePtr n : nt.children) {
    n->accept(*this);
    result_type& r = m_stack.top();
    m_stack.pop();
    make_edge(last, r.first);
    last = r.second;
  }

  make_edge(last, end);

  check_root(nt);
}

void RuleContext::root(rule_type& r) {
  m_root = proto::eval(r, *this);
  UpdateVisitor uv(m_named_nodes);
  m_root->accept(uv);
  Executor exec(m_root);
  m_root->accept(exec);
}

void RuleContext::print_dot_graph(rule_type& r, std::ostream& out) {
  NodePtr n = proto::eval(r, *this);
  UpdateVisitor uv(m_named_nodes);
  n->accept(uv);
  out << "digraph AST {" << std::endl;
  ToDotVisitor tdv(out);
  n->accept(tdv);
  out << "}" << std::endl;
}

void RuleContext::to_dot_file(rule_type& r, const char* filename) {
  std::fstream fs;
  fs.open(filename, std::fstream::out);
  print_dot_graph(r, fs);
  fs.close();
}

void RuleContext::display_graph(rule_type& r) {
  to_dot_file(r, "temp.dot");
  system("dot -Txlib temp.dot");
}

/**
 * Tests
 */
void test1() {
  RuleContext context;
  NAMED_RULE(r1);
  NAMED_RULE(r2);
  context(r1 = r2);  // failed
}

void test2() {
  RuleContext context;
  NAMED_RULE(r1);
  NAMED_RULE(r2);
  NAMED_RULE(r3);
  context(r1 = r2 >> r3);
  context(r1 = r3 >> r2);  // failed
}

void test3() {
  RuleContext context;
  NAMED_RULE(r1);
  NAMED_RULE(r2);
  NAMED_RULE(r3);
  context(r1 = r2 >> r3);
  context(r2 = r3 >> r1);
  context.root(r1);  // failed
}

void test4() {
  RuleContext context;

  NAMED_RULE(a);
  NAMED_RULE(b);
  NAMED_RULE(c);
  NAMED_RULE(d);
  NAMED_RULE(e);
  NAMED_RULE(f);
  NAMED_RULE(g);
  NAMED_RULE(h);
  NAMED_RULE(i);
  NAMED_RULE(j);

  context(a = b | c | d)(b = c >> d >> e)(c = f | g | (i >> j) | e)(d = i >> c >> h)(g = i | j)(h = (i >> j) |
                                                                                                    (j >> i));

  context.display_graph(a);
}
};
};
