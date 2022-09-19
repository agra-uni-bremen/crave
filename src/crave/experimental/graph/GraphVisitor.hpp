// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.//
#pragma once

#include <iostream>
#include <set>
#include <stack>
#include <string>

#include "Node.hpp"

namespace crave {

namespace graph {

/**
 * Visitor interface for class Node
 */
struct NodeVisitor {
  virtual void visitTerminal(Terminal&) = 0;
  virtual void visitSelector(Selector&) = 0;
  virtual void visitSequence(Sequence&) = 0;
};

/**
 * Visitor for class Node for printing
 */
struct PrintVisitor : NodeVisitor {
  PrintVisitor() : depth(0) {}

  virtual void visitTerminal(Terminal& t);
  virtual void visitSelector(Selector& sel);
  virtual void visitSequence(Sequence& seq);

 private:
  int depth;

 private:
  void printNode(Node& n);
};

/**
 * Node visitor to update Nodes
 */
struct UpdateVisitor : NodeVisitor {
  UpdateVisitor(node_map& m) : m_named_nodes(m), m_on_path() {}

  virtual void visitTerminal(Terminal& t);
  virtual void visitSelector(Selector& sel);
  virtual void visitSequence(Sequence& seq);

 private:
  node_map& m_named_nodes;
  std::set<std::string> m_on_path;

 private:
  void visitNonTerminal(NonTerminal& nt);
};

/**
 * Node visitor for convertion to Dot
 */
struct ToDotVisitor : NodeVisitor {
  typedef std::pair<int, int> result_type;

  ToDotVisitor(std::ostream& stream) : m_out(stream), m_stack(), m_node_count(0) {}

  virtual void visitTerminal(Terminal& t);
  virtual void visitSelector(Selector& nt);
  virtual void visitSequence(Sequence& nt);

 private:
  std::ostream& m_out;
  std::stack<result_type> m_stack;
  int m_node_count;

 private:
  void createNode(int ind, int id, const char* name, const char* shape, const char* color);
  void createEdge(int source, int dest, const char* color);
  void beginSubgraph(const char* name, const char* color, const char* style);
  void endSubgraph();
};
};  // namespace graph
};  // namespace crave
