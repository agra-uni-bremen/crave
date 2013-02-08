#ifndef ASSIGNRESULTIMPL_HPP_
#define ASSIGNRESULTIMPL_HPP_


#include <string>

#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/type_traits/is_signed.hpp>

#include "../crave/AssignResult.hpp"

namespace crave {

  template<typename T>
  struct AssignResultImpl : AssignResult {
  public:
    AssignResultImpl( T& value ) : value_(value) { }

  private:
    typedef boost::optional< boost::function0<bool> > Random;

    bool random_bit() const {
      return (*random_)();
    }

  public:
    virtual long value() const
    {
      return value_;
    };
    virtual void set_value( std::string const& str )
    {
      value_ = ((boost::is_signed<T>::value && str[0] == '1') ? -1: 0);
      for (std::string::const_iterator ite = str.begin(); ite != str.end(); ++ite)
      {
        value_ <<= 1;
        switch (*ite)
        {
          case '0': value_ &= T(-2); break;
          case '1': value_ |= T(1); break;
          default:
            if (random_ && random_bit())
              value_ |= T(1);
            else
              value_ &= T(-2);
            break;
        }
      }
      std::cout << "str: " << str << ", value: " << (long)value_ << std::endl;
    }

  protected:
    T & value_;
    Random random_;
  };

} /* namespace crave */

#endif /* ASSIGNRESULTIMPL_HPP_ */
