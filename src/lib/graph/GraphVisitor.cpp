#include "../../crave/graph/Node.hpp"
#include "../../crave/graph/GraphVisitor.hpp"

#include <boost/foreach.hpp>
#include <boost/assert.hpp>

namespace crave {

namespace graph {

void PrintVisitor::printNode(Node& n) {
  std::cout << std::string(depth, ' ') << n.kind() << " " << (n.name() ? n.name() : "") << std::endl;
}

void PrintVisitor::visitTerminal(Terminal& t) { ++depth, printNode(t), --depth; }

void PrintVisitor::visitSelector(Selector& sel) {
  ++depth;
  printNode(sel);
  BOOST_FOREACH(NodePtr n, sel.children)
  n->accept(*this);
  --depth;
}

void PrintVisitor::visitSequence(Sequence& seq) {
  ++depth;
  printNode(seq);
  BOOST_FOREACH(NodePtr n, seq.children)
  n->accept(*this);
  --depth;
}

void UpdateVisitor::visitTerminal(Terminal& t) {}

void UpdateVisitor::visitNonTerminal(NonTerminal& nt) {
  if (nt.name()) {
    BOOST_ASSERT_MSG(m_on_path.find(nt.name()) == m_on_path.end(), "Loop detected");
    m_on_path.insert(nt.name());
  }

  std::vector<NodePtr> updated_children;
  BOOST_FOREACH(NodePtr n, nt.children) {
    NodePtr tmp = n;
    if (n->name() && m_named_nodes.find(n->name()) != m_named_nodes.end()) {
      // update named child
      tmp = m_named_nodes[n->name()];
    }
    tmp->accept(*this);
    if (!tmp->name() && tmp->kind() == nt.kind()) {
      // unfold unnamed child of same kind
      NonTerminal& nt_child = static_cast<NonTerminal&>(*tmp);
      updated_children.insert(updated_children.end(), nt_child.children.begin(), nt_child.children.end());
      nt_child.children.clear();
    } else {
      updated_children.push_back(tmp);
    }
  }
  nt.children.clear();
  nt.children = updated_children;

  if (nt.name()) {
    m_on_path.erase(nt.name());
  }
}

void UpdateVisitor::visitSelector(Selector& sel) { visitNonTerminal(sel); }

void UpdateVisitor::visitSequence(Sequence& seq) { visitNonTerminal(seq); }

#define TDV_INDENT(x) std::string(m_stack.size() + x, '\t')

void ToDotVisitor::createNode(int ind, int id, const char* name, const char* shape, const char* color) {
  m_out << TDV_INDENT(ind) << id;
  m_out << " [";
  m_out << "label=\"" << name << "\" ";
  if (shape) m_out << "shape=" << shape << " ";
  if (color) m_out << "color=" << color << " ";
  m_out << "style=filled";
  m_out << "]" << std::endl;
}

void ToDotVisitor::createEdge(int source, int dest, const char* color) {
  m_out << TDV_INDENT(1) << source << " -> " << dest;
  if (color) m_out << "[color=" << color << "]";
  m_out << std::endl;
}

void ToDotVisitor::beginSubgraph(const char* name, const char* color, const char* style) {
  static unsigned int sg_cnt = 0;
  if (name) {
    m_out << TDV_INDENT(0) << "subgraph cluster_" << sg_cnt << " {" << std::endl;
    m_out << TDV_INDENT(1) << "label=\"" << name << "\";" << std::endl;
    if (color) m_out << TDV_INDENT(1) << "color=" << color << ";" << std::endl;
    if (style) m_out << TDV_INDENT(1) << "style=" << style << ";" << std::endl;
  } else {
    m_out << TDV_INDENT(0) << "subgraph sg_" << sg_cnt << " {" << std::endl;
  }
  m_out << TDV_INDENT(1) << "labeljust=l;" << std::endl;
  m_out << TDV_INDENT(1) << "fontsize=20.0;" << std::endl;
  sg_cnt++;
}

void ToDotVisitor::endSubgraph() { m_out << TDV_INDENT(0) << "}" << std::endl; }

void ToDotVisitor::visitTerminal(Terminal& t) {
  m_stack.push(result_type(m_node_count, m_node_count));
  createNode(0, m_node_count, t.name(), 0, "steelblue1");
  m_node_count++;
}

void ToDotVisitor::visitSelector(Selector& nt) {
  int start = m_node_count;
  int end = m_node_count + 1;
  m_node_count += 2;
  m_stack.push(result_type(start, end));

  beginSubgraph(nt.name(), "yellowgreen", "dashed");

  createNode(1, start, "start", "point", 0);
  createNode(1, end, "end", "point", 0);

  BOOST_FOREACH(NodePtr n, nt.children) {
    n->accept(*this);
    result_type& r = m_stack.top();
    m_stack.pop();
    createEdge(start, r.first, "azure4");
    createEdge(r.second, end, "azure4");
  }

  endSubgraph();
}

void ToDotVisitor::visitSequence(Sequence& nt) {
  int start = m_node_count;
  int end = m_node_count + 1;
  m_node_count += 2;
  m_stack.push(result_type(start, end));

  beginSubgraph(nt.name(), "yellowgreen", "dashed");

  createNode(1, start, "start", "point", 0);
  createNode(1, end, "end", "point", 0);

  int last = start;
  BOOST_FOREACH(NodePtr n, nt.children) {
    n->accept(*this);
    result_type& r = m_stack.top();
    m_stack.pop();
    createEdge(last, r.first, "azure4");
    last = r.second;
  }

  createEdge(last, end, "azure4");

  endSubgraph();
}

#undef TDV_INDENT
};
};
