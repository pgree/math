#ifndef STAN_MATH_PRIM_FUN_ASINH_HPP
#define STAN_MATH_PRIM_FUN_ASINH_HPP

#include <stan/math/prim/meta.hpp>
#include <cmath>

namespace stan {
namespace math {

/**
 * Structure to wrap asinh() so it can be vectorized.
 *
 * @tparam T argument scalar type
 * @param x argument
 * @return inverse hyperbolic sine of argument in radians.
 */
struct asinh_fun {
  template <typename T>
  static inline T fun(const T& x) {
    using std::asinh;
    return asinh(x);
  }
};

/**
 * Vectorized version of asinh().
 *
 * @tparam T type of container
 * @param x container
 * @return Inverse hyperbolic sine of each value in the container.
 */
template <typename T>
inline auto asinh(const T& x) {
  return apply_scalar_unary<asinh_fun, T>::apply(x);
}

}  // namespace math
}  // namespace stan

#endif
