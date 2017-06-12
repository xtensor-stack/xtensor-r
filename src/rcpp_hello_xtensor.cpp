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
xt::rtensor<double, 2> rcpp_hello_xtensor(xt::rtensor<int, 1> tens) {
	std::cout << tens << std::endl;
    auto t = xt::rtensor<double, 2>({{1,2,3},{5,5,5}});
    auto x = xt::rarray<double>({{1,2,3},{5,5,5}});

    xt::rarray<double> rarr = t * x + 2;
    std::cout << rarr << std::endl;
	return t * x;
}