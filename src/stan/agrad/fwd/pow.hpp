#ifndef __STAN__AGRAD__FWD__POW__HPP__
#define __STAN__AGRAD__FWD__POW__HPP__

#include <stan/agrad/fwd/fvar.hpp>
#include <stan/meta/traits.hpp>
#include <stan/agrad/fwd/inv.hpp>
#include <stan/agrad/fwd/inv_sqrt.hpp>
#include <stan/agrad/fwd/inv_square.hpp>

namespace stan{

  namespace agrad{

    template <typename T1, typename T2>
    inline
    fvar<typename stan::return_type<T1,T2>::type>
    pow(const fvar<T1>& x1, const T2& x2) {
      using std::pow;
      using stan::agrad::inv;
      using stan::agrad::inv_sqrt;
      using stan::agrad::inv_square;
      using stan::agrad::sqrt;
      using stan::agrad::square;
      if (x2 == -2)
        return inv_square(x1);
      if (x2 == -1)
        return inv(x1);
      if (x2 == -0.5)
        return inv_sqrt(x1);
      if (x2 == 0.5)
        return sqrt(x1);
      if (x2 == 1.0)
        return x1;
      if (x2 == 2.0)
        return square(x1);
      return fvar<typename 
                  stan::return_type<T1,T2>::type>(pow(x1.val_, x2),
                                           x1.d_ * x2 * pow(x1.val_, x2 - 1));
    }

    template <typename T1, typename T2>
    inline
    fvar<typename stan::return_type<T1,T2>::type>
    pow(const T1& x1, const fvar<T2>& x2) {
      using std::pow;
      using std::log;
      return fvar<typename 
                  stan::return_type<T1,T2>::type>(pow(x1, x2.val_),
                                          x2.d_ * log(x1) * pow(x1, x2.val_));
    }

    template <typename T1, typename T2>
    inline
    fvar<typename stan::return_type<T1,T2>::type>
    pow(const fvar<T1>& x1, const fvar<T2>& x2) {
      using std::pow;
      using std::log;
      return fvar<typename 
                  stan::return_type<T1,T2>::type>(pow(x1.val_, x2.val_),
                       (x2.d_ * log(x1.val_) 
                         + x2.val_ * x1.d_ / x1.val_) * pow(x1.val_, x2.val_));
    }
  }
}
#endif
