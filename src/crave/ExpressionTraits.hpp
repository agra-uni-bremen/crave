#pragma once

#include "bitsize_traits.hpp"

#include <boost/proto/core.hpp>
#include <boost/proto/transform.hpp>
#include <boost/proto/domain.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/min_max.hpp>
#include <boost/mpl/less.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/type_traits/is_signed.hpp>

namespace crave {
  namespace proto = boost::proto;

  struct extend_tag {};
  template<typename Stream>
  Stream & operator<< ( Stream & out, extend_tag const&) {
    return out << "extend_tag";
  }

  struct BooleanResult
  : proto::or_<
      // comparison
      proto::or_< 
        proto::nary_expr< proto::tag::less_equal, proto::vararg< proto::_> >
      , proto::nary_expr< proto::tag::less, proto::vararg< proto::_> >
      , proto::nary_expr< proto::tag::greater, proto::vararg< proto::_> >
      , proto::nary_expr< proto::tag::greater_equal, proto::vararg< proto::_> >
      , proto::nary_expr< proto::tag::equal_to, proto::vararg< proto::_> >
      , proto::nary_expr< proto::tag::not_equal_to, proto::vararg< proto::_> >
      >
      // logic      
    , proto::or_<
        proto::nary_expr< proto::tag::logical_and, proto::vararg< proto::_> >
      , proto::nary_expr< proto::tag::logical_or, proto::vararg< proto::_> >
      , proto::nary_expr< proto::tag::logical_not, proto::vararg< proto::_> >
      >
  > {};

  struct ExpressionSize
  : proto::or_<
    proto::when<proto::terminal<proto::_>, bitsize_traits<proto::_value>() >
  , proto::when<BooleanResult, boost::mpl::int_<1>() >
  , proto::when<proto::subscript< proto::_, proto::_ >, ExpressionSize(proto::_left) >
  , proto::when<
      proto::nary_expr<proto::_, proto::vararg<proto::_> >
    , proto::fold<proto::_, boost::mpl::int_<0>(), boost::mpl::max<ExpressionSize, proto::_state>()>
    >
  , proto::when<proto::_, boost::mpl::int_<0>() >
  > {};

  struct FixWidth;

  template<typename LEFT, typename RIGHT>
  struct Minus  {
    typedef boost::mpl::minus< 
            ExpressionSize(LEFT)
          , ExpressionSize(RIGHT) > type;
  };

  struct InjectExtend_ 
  : proto::when< proto::_, proto::binary_expr<extend_tag,
      proto::terminal< Minus< proto::_state, proto::_expr >::type () 
          > (Minus< proto::_state, proto::_expr >::type () )
      ,  proto::_expr 
      >( 
         proto::terminal< Minus< proto::_state, proto::_expr >::type () 
            > ( Minus< proto::_state, proto::_expr >::type () )
      ,  proto::_expr 
      )>
  {};

  struct FixFirstLarger 
  : proto::and_<
    proto::binary_expr<proto::_, proto::_, proto::_>
  , proto::if_< boost::mpl::less< ExpressionSize( proto::_left),  ExpressionSize( proto::_right) >() > 
  // transform to:
  , proto::when<proto::_, proto::binary_expr< 
      proto::tag_of< proto::_expr >(),
      InjectExtend_( FixWidth(proto::_left), proto::_right),
      FixWidth(proto::_right)
    > (
      InjectExtend_( FixWidth(proto::_left), proto::_right),
      FixWidth(proto::_right)
    ) >
  >{};


  struct FixSecondLarger 
  : proto::and_<
    proto::binary_expr<proto::_, proto::_, proto::_>
  , proto::if_< boost::mpl::greater< ExpressionSize( proto::_left),  ExpressionSize( proto::_right) >() > 
  // transform to:
  , proto::when<proto::_, proto::binary_expr< 
      proto::tag_of< proto::_expr >(),
      FixWidth(proto::_left),
      InjectExtend_( FixWidth(proto::_right), proto::_left)
    > (
      FixWidth(proto::_left),
      InjectExtend_( FixWidth(proto::_right), proto::_left)
    ) >
  >{};

  struct FixWidth 
  : proto::or_<
     proto::terminal<proto::_>
  ,  proto::subscript<proto::_, proto::_>
  ,  FixFirstLarger
  ,  FixSecondLarger
  ,  proto::nary_expr<proto::_, proto::vararg<FixWidth> >
  >{};

/***************************************************************************/
/*********** Sign of the expression ****************************************/
/***************************************************************************/

  struct FixedUnsigned
  : proto::or_<
      proto::nary_expr< proto::tag::less_equal, proto::vararg< proto::_> >
    , proto::nary_expr< proto::tag::less, proto::vararg< proto::_> >
    , proto::nary_expr< proto::tag::greater, proto::vararg< proto::_> >
    , proto::nary_expr< proto::tag::greater_equal, proto::vararg< proto::_> >
    , proto::nary_expr< proto::tag::equal_to, proto::vararg< proto::_> >
    , proto::nary_expr< proto::tag::not_equal_to, proto::vararg< proto::_> >
  > {};
  
  struct IsSigned
  : proto::and_<
      proto::not_< FixedUnsigned >
    , proto::or_ <
        proto::and_< 
          proto::terminal<proto::_> 
        , proto::if_< boost::is_signed< proto::_value > () >
        >
      , proto::subscript< IsSigned, proto::_ > 
      , proto::and_< 
          proto::not_< proto::subscript<proto::_, proto::_ > >
        , proto::when< proto::binary_expr<proto::_, proto::_, proto::_>         
        , proto::or_< 
            IsSigned( proto::_left )
          , IsSigned( proto::_right )
          >
        >
      >
    >
  > {};

/***************************************************************************/
/*********** Store the result in a variable ********************************/
/***************************************************************************/

  template <typename T>
  struct AssignResult {
    template <typename ResultWrapper>
    void operator() (T & target , ResultWrapper const & result) const
    {
      target = result;
    }
  };

} /* crave */

// vim: tabstop=2 shiftwidth=2 expandtab
