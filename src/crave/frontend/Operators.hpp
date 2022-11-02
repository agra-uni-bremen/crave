#include "../ir/Node.hpp"

namespace crave {
inline NodePtr varref(unsigned int id, unsigned int bs, bool s) { return std::make_shared<VariableExpr>(id, bs, s); }

inline NodePtr constant(uint64_t val, unsigned int bs, bool s) { return std::make_shared<Constant>(val, bs, s); }

template <typename T>
inline NodePtr constant(const T& val) {
  return std::make_shared<Constant>(to_constant_expr<T>{}(val));
}

#define NODE_BINOP(OP, OpNode) \
inline NodePtr operator OP(NodePtr left, NodePtr right) { return std::make_shared<OpNode>(left, right); } \
template <typename T> inline NodePtr operator OP(const T& left, NodePtr right) { return constant(left) OP right; } \
template <typename T> inline NodePtr operator OP(NodePtr left, const T& right) { return left OP constant(right); }

NODE_BINOP(+, PlusOpr)
NODE_BINOP(-, MinusOpr)
NODE_BINOP(*, MultipliesOpr)
NODE_BINOP(/, DivideOpr)
NODE_BINOP(%, ModuloOpr)
NODE_BINOP(==, EqualOpr)
NODE_BINOP(!=, NotEqualOpr)
NODE_BINOP(>, GreaterOpr)
NODE_BINOP(<, LessOpr)
NODE_BINOP(>=, GreaterEqualOpr)
NODE_BINOP(<=, LessEqualOpr)
NODE_BINOP(&&, LogicalAndOpr)
NODE_BINOP(||, LogicalOrOpr)
NODE_BINOP(&, AndOpr)
NODE_BINOP(|, OrOpr)
NODE_BINOP(^, XorOpr)

}  // namespace crave
