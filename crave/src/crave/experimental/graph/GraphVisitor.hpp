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
};
};
