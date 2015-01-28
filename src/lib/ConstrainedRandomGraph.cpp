#include <fstream>

#include "../crave/ConstrainedRandomGraph.hpp"
#include "../crave/graph/GraphVisitor.hpp"

namespace crave {

namespace graph {

void Terminal::accept(NodeVisitor& v) { v.visitTerminal(*this); }

void Selector::accept(NodeVisitor& v) { v.visitSelector(*this); }

void Sequence::accept(NodeVisitor& v) { v.visitSequence(*this); }

void RuleContext::root(rule_type & r) {
  NodePtr n = proto::eval(r, *this);
  UpdateVisitor uv(m_named_nodes);
  n->accept(uv);
  PrintVisitor pv;
  n->accept(pv);
}
  
void RuleContext::print_dot_graph(rule_type & r, std::ostream & out) {
  NodePtr n = proto::eval(r, *this);
  UpdateVisitor uv(m_named_nodes);
  n->accept(uv);
  out << "digraph AST {" << std::endl;
  ToDotVisitor tdv(out);
  n->accept(tdv);
  out << "}" << std::endl;
}

void RuleContext::to_dot_file(rule_type & r, const char* filename) {
  std::fstream fs;
  fs.open (filename, std::fstream::out);
  print_dot_graph(r, fs);
  fs.close();  
}

/**
 * Tests
 */
void test1() {
  RuleContext context;
  NAMED_RULE(r1);
  NAMED_RULE(r2);
  context(r1 = r2); // failed
}

void test2() {
  RuleContext context;
  NAMED_RULE(r1);
  NAMED_RULE(r2);
  NAMED_RULE(r3);
  context(r1 = r2 >> r3); 
  context(r1 = r3 >> r2); // failed
}

void test3() {
  RuleContext context;
  NAMED_RULE(r1);
  NAMED_RULE(r2);
  NAMED_RULE(r3);
  context(r1 = r2 >> r3); 
  context(r2 = r3 >> r1); 
  context.root(r1); // failed
}

};

};

