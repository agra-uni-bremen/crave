#pragma once

#include <boost/proto/core.hpp>

namespace platzhalter {
  namespace proto = boost::proto;

  struct Constraint_Domain;

  struct Constraint_Grammar
  : proto::_
  {};

  template<typename Expr>
  struct Constraint
  : proto::extends<Expr, Constraint<Expr>, Constraint_Domain >
  {
    typedef proto::extends<Expr, Constraint<Expr>, Constraint_Domain > base_type;

    Constraint(Expr const & expr = Expr()) 
      : base_type(expr)
    {
    }
  };


  struct Constraint_Domain
  : proto::domain<proto::generator<Constraint>, Constraint_Grammar>
  {};

   template<typename Expr> 
   void check (Constraint<Expr> const & ) {
     BOOST_MPL_ASSERT((proto::matches<Expr, Constraint_Grammar>));
   }

   template<typename Expr> 
   void check_not (Constraint<Expr> const & ) {
     BOOST_MPL_ASSERT_NOT((proto::matches<Expr, Constraint_Grammar>));
   }

  template<typename value_type, int id>
  struct var_tag {};

  template <typename OUT, typename value_type, int id>
  OUT & operator<< (OUT & out, var_tag<value_type, id> const & tag) {
    out << "var<" << id << ">" ;
    return out;
  }
 
  template<typename value_type_, int ID>
  struct Variable : public Constraint< typename boost::proto::terminal< var_tag<value_type_, ID> >::type >
  {
    typedef value_type_ value_type;
    static int id() { return ID; };
  };



} // namespace platzhalter

//  vim: ft=cpp:ts=2:sw=2:expandtab
