#include <gtest/gtest.h>
#include <stan/agrad/fvar.hpp>
#include <stan/agrad/var.hpp>
#include <test/agrad/util.hpp>

TEST(AgradFvar,gt) {
  using stan::agrad::fvar;
  fvar<double> v4 = 4;
  fvar<double> v5 = 5;
  double d4 = 4;
  double d5 = 5;
  
  EXPECT_TRUE(v5 > v4);
  EXPECT_TRUE(d5 > v4);
  EXPECT_TRUE(v5 > d4);
  EXPECT_TRUE(d5 > d4);

  int i4 = 4;
  int i5 = 5;

  EXPECT_TRUE(v5 > v4);
  EXPECT_TRUE(i5 > v4);
  EXPECT_TRUE(v5 > i4);
  EXPECT_TRUE(i5 > i4);
  EXPECT_TRUE(d5 > i4);
  EXPECT_TRUE(i5 > d4);
}

TEST(AgradFvarVar, gt) {
  using stan::agrad::fvar;
  using stan::agrad::var;

  fvar<var> x;
  x.val_ = 0.5;
  x.d_ = 1.3;

  fvar<var> y;
  y.val_ = 1.5;
  y.d_ = 1.0;

  fvar<var> z;
  z.val_ = 0.5;
  z.d_ = 1.3;

  EXPECT_FALSE(x > z);
  EXPECT_FALSE(x > y);
  EXPECT_FALSE(z > y);
}

TEST(AgradFvarFvar, gt) {
  using stan::agrad::fvar;

  fvar<fvar<double> > x;
  x.val_.val_ = 1.5;
  x.val_.d_ = 1.0;
  x.d_.val_ = 0.0;
  x.d_.d_ = 0.0;

  fvar<fvar<double> > y;
  y.val_.val_ = 0.5;
  y.val_.d_ = 0.0;
  y.d_.val_ = 1.0;
  y.d_.d_ = 0.0;

  fvar<fvar<double> > z;
  z.val_.val_ = 0.5;
  z.val_.d_ = 0.0;
  z.d_.val_ = 1.0;
  z.d_.d_ = 0.0;

  EXPECT_TRUE(x > y);
  EXPECT_TRUE(x > z);
  EXPECT_FALSE(z > y);
}

