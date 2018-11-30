// [[Rcpp::plugins(cpp14)]]

#include "xtensor-r/rcontainer.hpp"
#include "xtensor-r/rarray.hpp"
#include "xtensor/xio.hpp"

// [[Rcpp::export]]
int modify_cpp(xt::rarray<double>& x)
{
    // check that passing by reference works correctly
    x(0, 0)  = -1000;
    x(9, 2) = 1000;

    return 1;
}

// [[Rcpp::export]]
int reshape_cpp(xt::rarray<double>& x)
{
    // reshape in-place
    x.reshape({3, 10});
    return 1;
}

// [[Rcpp::export]]
xt::rarray<double> cpp_add(xt::rarray<double>& x, xt::rarray<double>& y)
{
    return x + y;
}

void xassert(bool cond)
{
    if (!cond) throw std::runtime_error("CPP ASSERT TRIGGERED.");
}

// [[Rcpp::export]]
int call_int(xt::rarray<int>& x)
{
    xassert(x(0, 0) == 1);
    xassert(x(0, 2) == 5);
    x(1, 1) = 35;
    return 1;
}

// [[Rcpp::export]]
int call_lgl(xt::rarray<rlogical>& x)
{
    xassert(x(0, 0) == 1);
    x(1, 1) = 0;
    return 1;
}

// [[Rcpp::export]]
int call_stdcomplex(xt::rarray<std::complex<double>>& x)
{
    using cplx = std::complex<double>;
    xassert(x(0, 0) == cplx(0., 1.));
    xassert(x(1, 2) == cplx(1., 5.));

    x(0, 2) = cplx(-10., -100.);
    return 1;
}
