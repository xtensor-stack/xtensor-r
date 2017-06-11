#include "xtensor-r/rcontainer.hpp"
#include "xtensor-r/rarray.hpp"
#include "xtensor-r/rtensor.hpp"

#include <xtensor/xio.hpp>
#include <xtensor/xinfo.hpp>
#include <xtensor/xarray.hpp>

#include <Rcpp.h>

using namespace Rcpp;

// [[Rcpp::plugins(cpp14)]]

// [[Rcpp::export]]
xt::rtensor<double, 2> rcpp_hello_xtensor(xt::rtensor<double, 3> tens) {
    auto t = xt::rtensor<double, 2>({{1,2,3},{5,5,5}});
    auto x = xt::rarray<double>({{1,2,3},{5,5,5}});

	return t * x;
    // std::cout << foo << std::endl;
    // std::cout << xt::info(foo) << std::endl;

    // std::vector<std::size_t> shp = {1,2,3};

    // xt::rarray<double> ttt(shp, 0.0);
    // ttt(0, 1) = 332211;
    // std::cout << ttt << std::endl;

    // std::cout << tens << std::endl;

    // xt::xarray<double> ar({{1,2,3,4}, {2,2,22,2}});
    // return tens;
}
