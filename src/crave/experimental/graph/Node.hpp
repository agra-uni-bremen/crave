// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.//
#pragma once

#include <map>
#include <memory>
#include <string>
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
};  // namespace graph
};  // namespace crave
