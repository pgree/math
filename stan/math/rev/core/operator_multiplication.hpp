#ifndef STAN_MATH_REV_CORE_OPERATOR_MULTIPLICATION_HPP
#define STAN_MATH_REV_CORE_OPERATOR_MULTIPLICATION_HPP

#include <stan/math/rev/core/var.hpp>
#include <stan/math/rev/core/vv_vari.hpp>
#include <stan/math/rev/core/vd_vari.hpp>
#include <stan/math/prim/meta.hpp>
#include <stan/math/prim/scal/fun/constants.hpp>
#include <stan/math/prim/scal/fun/is_any_nan.hpp>

namespace stan {
namespace math {

namespace internal {
class multiply_vv_vari : public op_vv_vari {
 public:
  multiply_vv_vari(vari* avi, vari* bvi)
      : op_vv_vari(avi->val_ * bvi->val_, avi, bvi) {}
  void chain() {
    if (unlikely(is_any_nan(avi_->val_, bvi_->val_))) {
      avi_->adj_ = NOT_A_NUMBER;
      bvi_->adj_ = NOT_A_NUMBER;
    } else {
      avi_->adj_ += bvi_->val_ * adj_;
      bvi_->adj_ += avi_->val_ * adj_;
    }
  }
};

class multiply_vd_vari : public op_vd_vari {
 public:
  multiply_vd_vari(vari* avi, double b) : op_vd_vari(avi->val_ * b, avi, b) {}
  void chain() {
    if (unlikely(is_any_nan(avi_->val_, bd_))) {
      avi_->adj_ = NOT_A_NUMBER;
    } else {
      avi_->adj_ += adj_ * bd_;
    }
  }
};
}  // namespace internal

/**
 * Multiplication operator for two variables (C++).
 *
 * The partial derivatives are
 *
 * \f$\frac{\partial}{\partial x} (x * y) = y\f$, and
 *
 * \f$\frac{\partial}{\partial y} (x * y) = x\f$.
 *
   \f[
   \mbox{operator*}(x, y) =
   \begin{cases}
     xy & \mbox{if } -\infty\leq x, y \leq \infty \\[6pt]
     \textrm{NaN} & \mbox{if } x = \textrm{NaN or } y = \textrm{NaN}
   \end{cases}
   \f]

   \f[
   \frac{\partial\, \mbox{operator*}(x, y)}{\partial x} =
   \begin{cases}
     y & \mbox{if } -\infty\leq x, y \leq \infty \\[6pt]
     \textrm{NaN} & \mbox{if } x = \textrm{NaN or } y = \textrm{NaN}
   \end{cases}
   \f]

   \f[
   \frac{\partial\, \mbox{operator*}(x, y)}{\partial y} =
   \begin{cases}
     x & \mbox{if } -\infty\leq x, y \leq \infty \\[6pt]
     \textrm{NaN} & \mbox{if } x = \textrm{NaN or } y = \textrm{NaN}
   \end{cases}
   \f]
 *
 * @tparam Var1 value type of a var
 * @tparam Var2 value type of a var
 * @param a First variable operand.
 * @param b Second variable operand.
 * @return Variable result of multiplying operands.
 */
template <typename Var1, typename Var2, require_all_var_t<Var1, Var2>...>
inline var operator*(Var1&& a, Var2&& b) {
  return {new internal::multiply_vv_vari(a.vi_, b.vi_)};
}

/**
 * Multiplication operator for a variable and a scalar (C++).
 *
 * The partial derivative for the variable is
 *
 * \f$\frac{\partial}{\partial x} (x * c) = c\f$, and
 *
 * @tparam Var value type of a var
 * @tparam Arith An arithmetic type
 * @param a Variable operand.
 * @param b Scalar operand.
 * @return Variable result of multiplying operands.
 */
template <typename Var, typename Arith, require_var_t<Var>...,
          require_arithmetic_t<Arith>...>
inline var operator*(Var&& a, Arith b) {
  if (b == 1.0) {
    return a;
  }
  return {new internal::multiply_vd_vari(a.vi_, b)};
}

/**
 * Multiplication operator for a scalar and a variable (C++).
 *
 * The partial derivative for the variable is
 *
 * \f$\frac{\partial}{\partial y} (c * y) = c\f$.
 *
 * @tparam Var value type of a var
 * @tparam Arith An arithmetic type
 * @param a Scalar operand.
 * @param b Variable operand.
 * @return Variable result of multiplying the operands.
 */
template <typename Arith, typename Var, require_arithmetic_t<Arith>...,
          require_var_t<Var>...>
inline var operator*(Arith a, Var&& b) {
  if (a == 1.0) {
    return b;
  }
  return {new internal::multiply_vd_vari(b.vi_, a)};  // by symmetry
}

}  // namespace math
}  // namespace stan
#endif
