#pragma once

#include <boost/proto/core.hpp>

using std::ostream;

namespace platzhalter {
  namespace proto = boost::proto;

  struct Constraint_Domain;

  struct Constraint_Grammar
  : proto::_
  {};

  template<typename T>
  class randv;

  template<typename T>
  struct Variable;

  template<typename Expr>
  struct Constraint
  : proto::extends<Expr, Constraint<Expr>, Constraint_Domain >
  {
    typedef proto::extends<Expr, Constraint<Expr>, Constraint_Domain > base_type;

    Constraint(Expr const & expr = Expr()) 
      : base_type(expr)
    {
    }

    
   template<typename T, typename E2>
   typename proto::result_of::make_expr< 
          proto::tag::assign
        , Constraint_Domain
        , Expr
        , E2
      > ::type 
      operator= ( E2 e ) {
      return proto::make_expr< proto::tag::assign>
        ( *this, e);
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
  struct write_ref_tag : public var_tag<value_type> { 
  write_ref_tag(int id_,  value_type & ref_) : var_tag<value_type>(id_), ref(ref_) { }; value_type & ref; };

  template<typename value_type>
  struct read_ref_tag : public var_tag<value_type> { 
  read_ref_tag(int id_,  value_type const & ref_) : var_tag<value_type>(id_), ref(ref_) { }; value_type const & ref; };

  struct randomize_tag { unsigned id; };
  const randomize_tag randomize = { 0 };

  template <typename OUT, typename value_type>
  OUT & operator<< (OUT & out, var_tag<value_type> const & tag) {
    out << "var<" << tag.id << ">" ;
    return out;
  }

  template <typename OUT>
  OUT & operator<< (OUT & out, randomize_tag const & tag ) {
    out << "randomize_tag <" << tag.id << ">"  ;
    return out;
  }
 
  int new_var_id() { static int _ID=0; return ++_ID;}

  template<typename value_type_>
  struct Variable : public Constraint< typename boost::proto::terminal< var_tag<value_type_> >::type >
  {
    typedef Constraint< typename proto::terminal< var_tag<value_type_> >::type > base_type;
    Variable() 
      : base_type( proto::make_expr< proto::tag::terminal>
        ( var_tag<value_type_>(new_var_id() ) ) )
    {}

    template<typename T>
    explicit Variable( T linkto ) 
    : Variable()
    { linkto.addVariable(*this); }

    typedef value_type_ value_type;
    int id() const {  return boost::proto::value(*this).id; };

   typename proto::result_of::make_expr< 
          proto::tag::assign
        , Constraint_Domain
        , Variable<value_type_> const & 
        , value_type_ const & 
      > ::type const
      operator= ( value_type_ const & e ) const {
      return proto::make_expr< proto::tag::assign, Constraint_Domain>
        ( boost::cref(*this), boost::cref(e));
      }

    proto::result_of::make_expr<
        proto::tag::terminal
      , Constraint_Domain
      , randomize_tag 
      > ::type
    operator= ( randomize_tag const & e ) const {
      const randomize_tag tag = { id() };
      return proto::make_expr< proto::tag::terminal > ( tag );
    }
  };

  template<typename value_type_>
  struct WriteReference : public Constraint< typename boost::proto::terminal< write_ref_tag<value_type_> >::type >
  {
    typedef Constraint< typename proto::terminal< write_ref_tag<value_type_> >::type > base_type;
    WriteReference(value_type_ & ref) 
      : base_type( proto::make_expr< proto::tag::terminal>
        ( write_ref_tag<value_type_>(new_var_id(), ref ) ) )
    {}
    typedef value_type_ value_type;
    int id() const {  return boost::proto::value(*this).id; };

   typename proto::result_of::make_expr< 
          proto::tag::assign
        , Constraint_Domain
        , WriteReference<value_type_> const & 
        , value_type_ const & 
      > ::type const
      operator= ( value_type_ const & e ) const {
      return proto::make_expr< proto::tag::assign, Constraint_Domain>
        ( boost::cref(*this), boost::cref(e));
      }

    proto::result_of::make_expr<
        proto::tag::terminal
      , Constraint_Domain
      , randomize_tag 
      > ::type
    operator= ( randomize_tag const & e ) const {
      const randomize_tag tag = { id() };
      return proto::make_expr< proto::tag::terminal > ( tag );
    }
  };


  template<typename value_type_>
  struct ReadReference : public Constraint< typename boost::proto::terminal< read_ref_tag<value_type_> >::type >
  {
    typedef Constraint< typename proto::terminal< read_ref_tag<value_type_> >::type > base_type;
    ReadReference(value_type_ &  ref) 
      : base_type( proto::make_expr< proto::tag::terminal>
        ( read_ref_tag<value_type_>(new_var_id(), ref) ) )
    {}
    typedef value_type_ value_type;
    int id() const {  return boost::proto::value(*this).id; };
  };

      template<typename T>
      typename proto::result_of::make_expr< 
          proto::tag::terminal
        , Constraint_Domain
        , read_ref_tag<T>
      > ::type 
      reference( T const &  ref )
      {
        return proto::make_expr< proto::tag::terminal, Constraint_Domain >( read_ref_tag<T>( new_var_id(), ref )) ;
      } 

} // namespace platzhalter

//  vim: ft=cpp:ts=2:sw=2:expandtab
