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

  struct extend_tag {};
  template<typename Stream>
  Stream & operator<< ( Stream & out, extend_tag const&) {
    return out << "extend_tag";
  }

  struct BooleanResult
  : boost::proto::or_<
      // comparison
      boost::proto::or_<
        boost::proto::nary_expr< boost::proto::tag::less_equal, boost::proto::vararg< boost::proto::_> >
      , boost::proto::nary_expr< boost::proto::tag::less, boost::proto::vararg< boost::proto::_> >
      , boost::proto::nary_expr< boost::proto::tag::greater, boost::proto::vararg< boost::proto::_> >
      , boost::proto::nary_expr< boost::proto::tag::greater_equal, boost::proto::vararg< boost::proto::_> >
      , boost::proto::nary_expr< boost::proto::tag::equal_to, boost::proto::vararg< boost::proto::_> >
      , boost::proto::nary_expr< boost::proto::tag::not_equal_to, boost::proto::vararg< boost::proto::_> >
      >
      // logic
    , boost::proto::or_<
        boost::proto::nary_expr< boost::proto::tag::logical_and, boost::proto::vararg< boost::proto::_> >
      , boost::proto::nary_expr< boost::proto::tag::logical_or, boost::proto::vararg< boost::proto::_> >
      , boost::proto::nary_expr< boost::proto::tag::logical_not, boost::proto::vararg< boost::proto::_> >
      >
      // special operators
    , boost::proto::nary_expr< boost::proto::tag::function, boost::proto::terminal<crave::operator_foreach>, boost::proto::_, boost::proto::_ >
    , boost::proto::nary_expr< boost::proto::tag::function, boost::proto::terminal<crave::operator_unique>, boost::proto::_ >
  > {};

  struct ExpressionSize
  : boost::proto::or_<
    boost::proto::when<BooleanResult, boost::mpl::int_<1>() >
  , boost::proto::when<boost::proto::function< boost::proto::terminal<operator_bitslice> , boost::proto::_, boost::proto::_, boost::proto::_ > , ExpressionSize(boost::proto::_child_c<3>) >
  , boost::proto::when<boost::proto::terminal<boost::proto::_>, bitsize_traits<boost::proto::_value>() >
  , boost::proto::when<boost::proto::subscript< boost::proto::_, boost::proto::_ >, ExpressionSize(boost::proto::_left) >
  , boost::proto::when<boost::proto::binary_expr< extend_tag,  boost::proto::_, boost::proto::_ >,
      boost::mpl::int_<0>()
    >
  , boost::proto::when<
      boost::proto::nary_expr<boost::proto::_, boost::proto::vararg<boost::proto::_> >
    , boost::proto::fold<boost::proto::_, boost::mpl::int_<0>(), boost::mpl::max<ExpressionSize, boost::proto::_state>()>
    >
  , boost::proto::when<boost::proto::_, boost::mpl::int_<0>() >
  > {};

  struct FixWidth;

  template<typename LEFT, typename RIGHT>
  struct Minus  {
    typedef boost::mpl::minus<
            ExpressionSize(LEFT)
          , ExpressionSize(RIGHT) > type;
  };

  struct InjectExtend_
  : boost::proto::when< boost::proto::_, boost::proto::binary_expr<extend_tag,
      boost::proto::terminal< Minus< boost::proto::_state, boost::proto::_expr >::type ()
          > (Minus< boost::proto::_state, boost::proto::_expr >::type () )
      ,  boost::proto::_expr
      >(
         boost::proto::terminal< Minus< boost::proto::_state, boost::proto::_expr >::type ()
            > ( Minus< boost::proto::_state, boost::proto::_expr >::type () )
      ,  boost::proto::_expr
      )>
  {};

  struct FixFirstLarger
  : boost::proto::and_<
    boost::proto::binary_expr<boost::proto::_, boost::proto::_, boost::proto::_>
  , boost::proto::if_< boost::mpl::less< ExpressionSize( boost::proto::_left),  ExpressionSize( boost::proto::_right) >() >
  // transform to:
  , boost::proto::when<boost::proto::_, boost::proto::binary_expr<
      boost::proto::tag_of< boost::proto::_expr >(),
      InjectExtend_( FixWidth(boost::proto::_left), boost::proto::_right),
      FixWidth(boost::proto::_right)
    > (
      InjectExtend_( FixWidth(boost::proto::_left), boost::proto::_right),
      FixWidth(boost::proto::_right)
    ) >
  >{};


  struct FixSecondLarger
  : boost::proto::and_<
    boost::proto::binary_expr<boost::proto::_, boost::proto::_, boost::proto::_>
  , boost::proto::if_< boost::mpl::greater< ExpressionSize( boost::proto::_left),  ExpressionSize( boost::proto::_right) >() >
  // transform to:
  , boost::proto::when<boost::proto::_, boost::proto::binary_expr<
      boost::proto::tag_of< boost::proto::_expr >(),
      FixWidth(boost::proto::_left),
      InjectExtend_( FixWidth(boost::proto::_right), boost::proto::_left)
    > (
      FixWidth(boost::proto::_left),
      InjectExtend_( FixWidth(boost::proto::_right), boost::proto::_left)
    ) >
  >{};

  template<typename T>
  struct distribution;

  struct FixWidth
  : boost::proto::or_<
     boost::proto::terminal<boost::proto::_>
  ,  boost::proto::subscript<boost::proto::_, boost::proto::_>
  ,  boost::proto::nary_expr< boost::proto::tag::function, boost::proto::terminal<crave::operator_unique>, boost::proto::_ >
  ,  boost::proto::nary_expr< boost::proto::tag::function, boost::proto::terminal<crave::operator_dist>, boost::proto::_,       
                                                           boost::proto::terminal<crave::distribution<boost::proto::_> > >
  ,  FixFirstLarger
  ,  FixSecondLarger
  ,  boost::proto::nary_expr<boost::proto::_, boost::proto::vararg<FixWidth> >
  >{};

/***************************************************************************/
/*********** Sign of the expression ****************************************/
/***************************************************************************/

  struct FixedUnsigned
  : boost::proto::or_<
      boost::proto::nary_expr< boost::proto::tag::less_equal, boost::proto::vararg< boost::proto::_> >
    , boost::proto::nary_expr< boost::proto::tag::less, boost::proto::vararg< boost::proto::_> >
    , boost::proto::nary_expr< boost::proto::tag::greater, boost::proto::vararg< boost::proto::_> >
    , boost::proto::nary_expr< boost::proto::tag::greater_equal, boost::proto::vararg< boost::proto::_> >
    , boost::proto::nary_expr< boost::proto::tag::equal_to, boost::proto::vararg< boost::proto::_> >
    , boost::proto::nary_expr< boost::proto::tag::not_equal_to, boost::proto::vararg< boost::proto::_> >
    , boost::proto::function< boost::proto::terminal<operator_bitslice> , boost::proto::_, boost::proto::_, boost::proto::_ > 
  > {};

  struct IsSigned
  : boost::proto::and_<
      boost::proto::not_< FixedUnsigned >
    , boost::proto::or_ <
        boost::proto::and_<
          boost::proto::terminal<boost::proto::_>
        , boost::proto::if_< boost::is_signed< boost::proto::_value > () >
        >
      , boost::proto::subscript< IsSigned, boost::proto::_ >
      , boost::proto::and_<
          boost::proto::not_< boost::proto::subscript<boost::proto::_, boost::proto::_ > >
        , boost::proto::when< boost::proto::binary_expr<boost::proto::_, boost::proto::_, boost::proto::_>
        , boost::proto::or_<
            IsSigned( boost::proto::_left )
          , IsSigned( boost::proto::_right )
          >
        >
      >
    >
  > {};

} /* crave */

// vim: tabstop=2 shiftwidth=2 expandtab
