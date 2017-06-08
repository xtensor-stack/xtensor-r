#include "xtensor.h"

#include <xtensor/xio.hpp>
#include <xtensor/xinfo.hpp>
#include <xtensor/xarray.hpp>

#include <Rcpp.h>

using namespace Rcpp;

// [[Rcpp::plugins(cpp14)]]

// [[Rcpp::export]]
xt::rxarray<double> rcpp_hello_xtensor(xt::rxarray<double>& foo) {

    // std::cout << foo << std::endl;
    // std::cout << xt::info(foo) << std::endl;

    std::vector<std::size_t> shp = {1,2,3};

    xt::rxarray<double> ttt(shp, 0.0);
    ttt(0, 1) = 332211;
    // std::cout << ttt << std::endl;

    xt::xarray<double> ar({{1,2,3,4}, {2,2,22,2}});
    return ar;
}
