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
  namespace bproto = boost::proto;

  struct extend_tag {};
  template<typename Stream>
  Stream & operator<< ( Stream & out, extend_tag const&) {
    return out << "extend_tag";
  }

  struct BooleanResult
  : bproto::or_<
      // comparison
      bproto::or_<
        bproto::nary_expr< bproto::tag::less_equal, bproto::vararg< bproto::_> >
      , bproto::nary_expr< bproto::tag::less, bproto::vararg< bproto::_> >
      , bproto::nary_expr< bproto::tag::greater, bproto::vararg< bproto::_> >
      , bproto::nary_expr< bproto::tag::greater_equal, bproto::vararg< bproto::_> >
      , bproto::nary_expr< bproto::tag::equal_to, bproto::vararg< bproto::_> >
      , bproto::nary_expr< bproto::tag::not_equal_to, bproto::vararg< bproto::_> >
      >
      // logic
    , bproto::or_<
        bproto::nary_expr< bproto::tag::logical_and, bproto::vararg< bproto::_> >
      , bproto::nary_expr< bproto::tag::logical_or, bproto::vararg< bproto::_> >
      , bproto::nary_expr< bproto::tag::logical_not, bproto::vararg< bproto::_> >
      >
  > {};

  struct ExpressionSize
  : bproto::or_<
    bproto::when<bproto::terminal<bproto::_>, bitsize_traits<bproto::_value>() >
  , bproto::when<BooleanResult, boost::mpl::int_<1>() >
  , bproto::when<bproto::subscript< bproto::_, bproto::_ >, ExpressionSize(bproto::_left) >
  , bproto::when<bproto::binary_expr< extend_tag,  bproto::_, bproto::_ >,
      boost::mpl::int_<0>()
    >
  , bproto::when<
      bproto::nary_expr<bproto::_, bproto::vararg<bproto::_> >
    , bproto::fold<bproto::_, boost::mpl::int_<0>(), boost::mpl::max<ExpressionSize, bproto::_state>()>
    >
  , bproto::when<bproto::_, boost::mpl::int_<0>() >
  > {};

  struct FixWidth;

  template<typename LEFT, typename RIGHT>
  struct Minus  {
    typedef boost::mpl::minus<
            ExpressionSize(LEFT)
          , ExpressionSize(RIGHT) > type;
  };

  struct InjectExtend_
  : bproto::when< bproto::_, bproto::binary_expr<extend_tag,
      bproto::terminal< Minus< bproto::_state, bproto::_expr >::type ()
          > (Minus< bproto::_state, bproto::_expr >::type () )
      ,  bproto::_expr
      >(
         bproto::terminal< Minus< bproto::_state, bproto::_expr >::type ()
            > ( Minus< bproto::_state, bproto::_expr >::type () )
      ,  bproto::_expr
      )>
  {};

  struct FixFirstLarger
  : bproto::and_<
    bproto::binary_expr<bproto::_, bproto::_, bproto::_>
  , bproto::if_< boost::mpl::less< ExpressionSize( bproto::_left),  ExpressionSize( bproto::_right) >() >
  // transform to:
  , bproto::when<bproto::_, bproto::binary_expr<
      bproto::tag_of< bproto::_expr >(),
      InjectExtend_( FixWidth(bproto::_left), bproto::_right),
      FixWidth(bproto::_right)
    > (
      InjectExtend_( FixWidth(bproto::_left), bproto::_right),
      FixWidth(bproto::_right)
    ) >
  >{};


  struct FixSecondLarger
  : bproto::and_<
    bproto::binary_expr<bproto::_, bproto::_, bproto::_>
  , bproto::if_< boost::mpl::greater< ExpressionSize( bproto::_left),  ExpressionSize( bproto::_right) >() >
  // transform to:
  , bproto::when<bproto::_, bproto::binary_expr<
      bproto::tag_of< bproto::_expr >(),
      FixWidth(bproto::_left),
      InjectExtend_( FixWidth(bproto::_right), bproto::_left)
    > (
      FixWidth(bproto::_left),
      InjectExtend_( FixWidth(bproto::_right), bproto::_left)
    ) >
  >{};

  struct FixWidth
  : bproto::or_<
     bproto::terminal<bproto::_>
  ,  bproto::subscript<bproto::_, bproto::_>
  ,  FixFirstLarger
  ,  FixSecondLarger
  ,  bproto::nary_expr<bproto::_, bproto::vararg<FixWidth> >
  >{};

/***************************************************************************/
/*********** Sign of the expression ****************************************/
/***************************************************************************/

  struct FixedUnsigned
  : bproto::or_<
      bproto::nary_expr< bproto::tag::less_equal, bproto::vararg< bproto::_> >
    , bproto::nary_expr< bproto::tag::less, bproto::vararg< bproto::_> >
    , bproto::nary_expr< bproto::tag::greater, bproto::vararg< bproto::_> >
    , bproto::nary_expr< bproto::tag::greater_equal, bproto::vararg< bproto::_> >
    , bproto::nary_expr< bproto::tag::equal_to, bproto::vararg< bproto::_> >
    , bproto::nary_expr< bproto::tag::not_equal_to, bproto::vararg< bproto::_> >
  > {};

  struct IsSigned
  : bproto::and_<
      bproto::not_< FixedUnsigned >
    , bproto::or_ <
        bproto::and_<
          bproto::terminal<bproto::_>
        , bproto::if_< boost::is_signed< bproto::_value > () >
        >
      , bproto::subscript< IsSigned, bproto::_ >
      , bproto::and_<
          bproto::not_< bproto::subscript<bproto::_, bproto::_ > >
        , bproto::when< bproto::binary_expr<bproto::_, bproto::_, bproto::_>
        , bproto::or_<
            IsSigned( bproto::_left )
          , IsSigned( bproto::_right )
          >
        >
      >
    >
  > {};

} /* crave */

// vim: tabstop=2 shiftwidth=2 expandtab
