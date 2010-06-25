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

  template<typename value_type>
  struct var_tag { var_tag(int id_): id(id_) { }; int id; };

  template<typename value_type>
  struct ref_tag : public var_tag<value_type> { 
  ref_tag(int id_,  value_type const & ref_) : var_tag<value_type>(id_), ref(ref_) { }; value_type const & ref; };

  template <typename OUT, typename value_type>
  OUT & operator<< (OUT & out, var_tag<value_type> const & tag) {
    out << "var<" << tag.id << ">" ;
    return out;
  }
 
  int new_var_id() { static int _ID=0; return ++_ID;}

  template<typename value_type_>
  struct Variable : public Constraint< typename boost::proto::terminal< var_tag<value_type_> >::type >
  {
    typedef Constraint< typename proto::terminal< var_tag<value_type_> >::type > base_type;
    Variable() 
      : base_type( proto::make_expr< proto::tag::terminal>
        ( var_tag<value_type_>(new_var_id()) ) )
    {}
    typedef value_type_ value_type;
    int id() const {  return boost::proto::value(*this).id; };
  };


      template<typename T>
      typename proto::result_of::make_expr< 
          proto::tag::terminal
        , Constraint_Domain
        , ref_tag<T>
      > ::type 
      reference( T const &  ref )
      {
        return proto::make_expr< proto::tag::terminal, Constraint_Domain >( ref_tag<T>( new_var_id(), ref )) ;
      } 


} // namespace platzhalter

//  vim: ft=cpp:ts=2:sw=2:expandtab
