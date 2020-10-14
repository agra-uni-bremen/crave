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

#include <string>
#include <map>
#include <memory>
#include <vector>

namespace crave {

namespace graph {

struct Node;
struct Terminal;
struct Selector;
struct Sequence;
typedef std::shared_ptr<Node> NodePtr;
typedef std::map<std::string, NodePtr> node_map;

struct NodeVisitor;

/**
 * Graph Node
 */
struct Node {
  /**
   * constructor
   * @param name name of the Node
  */
  Node(const char* name = 0) : m_name(name) {}

  /**
   * get node name
   * @return name of this node
   */
  const char* name() const { return m_name; }

  /**
   * function to rename this node
   * @param name new name of this node
   */
  void rename(const char* name) { m_name = name; }

  /**
   * get object kind
   * @return object kind as string
   */
  virtual const char* obj_kind() const = 0;

  /**
   * accept function for visitor pattern
   */
  virtual void accept(NodeVisitor&) = 0;

  /**
   * destructor
   */
  virtual ~Node() {}

 protected:
  const char* m_name;
};

struct Terminal : Node {
  Terminal(const char* name) : Node(name) {}
  virtual ~Terminal() {}
  virtual const char* obj_kind() const { return "Terminal"; }
  virtual void accept(NodeVisitor& v);
};

struct NonTerminal : Node {
  void add(NodePtr c) { children.push_back(c); }
  std::vector<NodePtr> children;
};

struct Selector : NonTerminal {
  virtual const char* obj_kind() const { return "Selector"; }
  virtual void accept(NodeVisitor& v);
  virtual ~Selector() {}
};

struct Sequence : NonTerminal {
  virtual const char* obj_kind() const { return "Sequence"; }
  virtual void accept(NodeVisitor& v);
};
};
};
