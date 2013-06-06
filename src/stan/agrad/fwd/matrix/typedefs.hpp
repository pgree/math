#ifndef __STAN__AGRAD__FWD__MATRIX__TYPEDEFS_HPP__
#define __STAN__AGRAD__FWD__MATRIX__TYPEDEFS_HPP__

#include <stan/math/matrix/Eigen.hpp>
#include <stan/agrad/fwd/fvar.hpp>
#include <stan/agrad/rev/var.hpp>
#include <stan/agrad/fwd/numeric_limits.hpp>

namespace stan {
  namespace agrad {
    
    typedef 
    Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic>::Index 
    size_type;

    typedef 
    Eigen::Matrix<fvar<double>,Eigen::Dynamic,Eigen::Dynamic>
    matrix_fv;

    typedef 
    Eigen::Matrix<fvar<var>,Eigen::Dynamic,Eigen::Dynamic>
    matrix_fvv;

    typedef 
    Eigen::Matrix<fvar<fvar<double> >,Eigen::Dynamic,Eigen::Dynamic>
    matrix_ffv;

    typedef 
    Eigen::Matrix<fvar<double>,Eigen::Dynamic,1>
    vector_fv;

    typedef 
    Eigen::Matrix<fvar<var>,Eigen::Dynamic,1>
    vector_fvv;

    typedef 
    Eigen::Matrix<fvar<fvar<double> >,Eigen::Dynamic,1>
    vector_ffv;

    typedef 
    Eigen::Matrix<fvar<double>,1,Eigen::Dynamic>
    row_vector_fv;

    typedef 
    Eigen::Matrix<fvar<var>,1,Eigen::Dynamic>
    row_vector_fvv;

    typedef 
    Eigen::Matrix<fvar<fvar<double> >,1,Eigen::Dynamic>
    row_vector_ffv;

  }
}
#endif
