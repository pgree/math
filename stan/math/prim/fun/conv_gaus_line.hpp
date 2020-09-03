#ifndef STAN_MATH_PRIM_FUN_CONV_GAUS_LINE
#define STAN_MATH_PRIM_FUN_CONV_GAUS_LINE

#include <stan/math/prim/meta.hpp>
#include <stan/math/prim/prob/normal_cdf.hpp>
#include <cmath>
#include <vector>

namespace stan {
namespace math {

/**
 * Evaluate the convolution of a line with a Gaussian kernel on an interval.
 *
 * \f$\int_{t_0}^{t_1} (at + b) e^{\frac{-(t-x)^2}{2\sigma^2}} dt \f$
 *
 * @param t0 lower integration bound
 * @param t1 upper integration bound
 * @param a coefficient of t in line
 * @param b constant in line
 * @param x point at which convolution is evaluated
 * @param sig2 variance of the Gaussian kernel
 * @return The value of the derivative
 */
template <typename Tx>
inline return_type_t<Tx> conv_gaus_line(double t0, double t1, double a, double b,
					const Tx& x, double sig2) {
  using stan::math::normal_cdf;
  using std::exp;
  using std::pow;
  using std::sqrt;
  const double sig = sqrt(sig2);
  const double pi = stan::math::pi();

  return_type_t<Tx> y
      = (a * x + b)
        * (normal_cdf(t1, x, sig) - normal_cdf(t0, x, sig));
  y += -a * sig2 / sqrt(2 * pi * sig2)
       * (exp(-pow(t1 - x, 2) / (2 * sig2))
          - exp(-pow(t0 - x, 2) / (2 * sig2)));
  return y;
}

}  // namespace math
}  // namespace stan

#endif
