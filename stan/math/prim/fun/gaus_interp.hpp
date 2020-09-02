#ifndef STAN_MATH_PRIM_FUN_GAUS_INTERP
#define STAN_MATH_PRIM_FUN_GAUS_INTERP

#include <stan/math/prim/fun/conv_gaus_line.hpp>
#include <stan/math/prim/fun/square.hpp>
#include <stan/math/prim/err.hpp>
#include <cmath>
#include <vector>
#include <algorithm>

namespace stan {
namespace math {

namespace internal {
/*
 * find the smallest difference between successive elements in a sorted vector
 */
template <typename Tx>
inline double min_diff(int n, const std::vector<Tx>& xs) {
  double dmin = value_of(xs[1]) - value_of(xs[0]);
  for (int i = 1; i < n - 1; i++) {
    if (value_of(xs[i + 1]) - value_of(xs[i]) < dmin) {
      dmin = value_of(xs[i + 1]) - value_of(xs[i]);
    }
  }
  return dmin;
}

}  // namespace internal

/**
 * Given a set of reference points \f$(xs_i, ys_i)\f$, create a mollifier
 * that intersects the reference points. This function requires as input
 * a struct created by the function gaus_interp_precomp. The algorithm
 * used to create the mollifier is an iterative algorithm that works
 * as follows. First a linear
 * interpolation is created through the reference points. Then, the
 * linear interpolation is convolved with a Gaussian whose width is
 * proportional the smallest distance between successive points
 * \f$xs_i\f$ and \f$xs_{i+1}\f$. Since the convolution is unlikely to
 * intersect the reference points, the y-values of the reference points
 * are shifted and the process repeats itself until the interpolation
 * intersects all reference points.
 *
 * Note: This interpolation scheme should be used when the function
 * to be interpolated is well-resolved by the reference points.
 *
 * @param xs vector of independent variable of reference points
 * @param ys vector of dependent variable of reference points
 * @param params a vector created by gaus_interp_precomp
 * @param x the point at which to evaluate the interpolation
 * @return value of the interpolation at x
 */
template <typename Tx>
inline return_type_t<Tx> gaus_interp(const std::vector<double>& xs,
                                     const std::vector<double>& ys,
                                     const std::vector<double>& params,
                                     const Tx& x) {
  // enforce that interpolation point is between smallest and largest
  // reference point
  static char const* function = "gaus_interp";
  check_less_or_equal(function, "Interpolation point", x, xs.back());
  check_greater_or_equal(function, "Interpolation point", x, xs.front());
  check_ordered(function, "xs", xs);
  check_not_nan(function, "xs", xs);
  check_not_nan(function, "ys", ys);
  check_not_nan(function, "x", x);
  check_greater(function, "xs", xs.size(), 1);

  // number of standard deviations to extend endpoints for convolution
  const double NSTDS = 10;
  int n = xs.size();

  // params is vector of the form (as, bs, sig2)

  // create copy of xs so that endpoints can be extended
  std::vector<double> xs2 = xs;

  // extend out first and last lines for convolution
  double sig = std::sqrt(params[2*n-2]);
  xs2[0] = xs[0] - NSTDS * sig;
  xs2[n - 1] = xs[n - 1] + NSTDS * sig;

  // no need to convolve far from center of gaussian, so
  // get lower and upper indexes for integration bounds
  auto lb = std::lower_bound(xs.begin(), xs.end(), x - NSTDS * sig);
  int ind_start = &(*lb) - &xs[0] - 1;
  ind_start = std::max(0, ind_start);

  auto ub = std::upper_bound(xs.begin(), xs.end(), x + NSTDS * sig);
  int ind_end = &(*ub) - &xs[0];
  ind_end = std::min(n - 1, ind_end);

  // sum convolutions over intervals
  /*
  return_type_t<Tx> y = 0;
  for (int i = ind_start; i < ind_end; i++) {
    y += conv_gaus_line(xs2[i], xs2[i + 1], params[i], params[(n-1) + i], x,
                        params[2*n-2]);
  }
  */
  return_type_t<Tx> y = conv_gaus_line_sum(x, xs2, params, ind_start, ind_end);

  return y;
}

/**
 * This function was written to be used with gaus_interp. This function
 * computes the shifted y-values of the reference points of an interpolation
 * in such a way that when that piecewise linear function is convolved
 * with a Gaussian kernel, the resulting function coincides with the
 * points \f$(xs_i, ys_i)\f$ inputted into this function. The output of this
 * function depends heavily on the choice of width of the Gaussian
 * kernel, which at the time of writing, is set to one tenth the
 * minimum distance between successive elements of the vector xs.
 * A tolerance for the maximum distance between the interpolation and
 * all reference points is also set manually and is not an input.
 *
 *
 * @param xs vector of independent variable of reference points
 * @param ys vector of dependent variable of reference points
 * @return struct containing slopes, intercepts, and width of kernel
 */
inline std::vector<double> gaus_interp_precomp(const std::vector<double>& xs,
					       const std::vector<double>& ys) {
  static char const* function = "gaus_interp_precomp";
  check_not_nan(function, "xs", xs);
  check_not_nan(function, "ys", ys);
  check_ordered(function, "xs", xs);
  check_greater(function, "xs", xs.size(), 1);

  using internal::min_diff;
  static const double max_diff = 1e-8;
  static const double sig2_scale = 0.1;
  int n = xs.size();

  // create the vector to be returned that consists of as, bs, sig2
  std::vector<double> params;
  params.resize(2*n-1);
  params[2*n-2] = square(min_diff(n, xs) * sig2_scale);

  // copy ys into a new vector that will be changed
  std::vector<double> y2s = ys;

  // interatively find interpolation that coincides with ys at xs
  int max_iters = 50;
  double dd;
  for (int j = 0; j < max_iters; j++) {
    // find slope and intercept of line between each point
    for (size_t i = 0; i < n - 1; i++) {
      params[i] = (y2s[i + 1] - y2s[i]) / (xs[i + 1] - xs[i]);
      params[(n - 1) + i] = -xs[i] * params[i] + y2s[i];
    }

    double dmax = 0;
    for (size_t i = 0; i < n; i++) {
      dd = ys[i] - gaus_interp(xs, y2s, params, xs[i]);
      y2s[i] += dd;
      dmax = std::max(std::abs(dd), dmax);
    }
    if (dmax < max_diff)
      break;
  }
  return params;
}

/**
 * This function combines gaus_interp_precomp and gaus_interp.
 * It takes as input two vectors of reference points (xs and ys)
 * in addition to a vector, xs_new, of points at which this
 * function will evaluate the interpolation through those reference
 * points.
 *
 * @param xs vector of independent variable of reference points
 * @param ys vector of dependent variable of reference points
 * @param xs_new vector of point at which to evaluate interpolation
 * @return vector of interpolation values
 */

template <typename Tx>
inline std::vector<Tx> gaus_interp(const std::vector<double>& xs,
				   const std::vector<double>& ys,
				   const std::vector<Tx>& xs_new) {
  int n_interp = xs_new.size();
  std::vector<Tx> ys_new(n_interp);

  // create interpolation
  std::vector<double> params = gaus_interp_precomp(xs, ys);
  for (int i = 0; i < n_interp; i++) {
    ys_new[i] = gaus_interp(xs, ys, params, xs_new[i]);
  }
  return ys_new;
}

}  // namespace math
}  // namespace stan

#endif
