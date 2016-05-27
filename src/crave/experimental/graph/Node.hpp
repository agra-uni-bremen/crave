#pragma once

#include <string>
#include <map>
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

struct Node {
  Node(const char* name = 0) : m_name(name) {}

  const char* name() const { return m_name; }
  void rename(const char* name) { m_name = name; }

  virtual const char* obj_kind() const = 0;
  virtual void accept(NodeVisitor&) = 0;
  virtual ~Node(){}
 protected:
  const char* m_name;
};

struct Terminal : Node {
  Terminal(const char* name) : Node(name) {}
  virtual ~Terminal(){}
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
  virtual ~Selector(){}
};

struct Sequence : NonTerminal {
  virtual const char* obj_kind() const { return "Sequence"; }
  virtual void accept(NodeVisitor& v);
};
};
};
