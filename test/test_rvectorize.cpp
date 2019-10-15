/***************************************************************************
* Copyright (c) Wolf Vollprecht, Johan Mabille, and Sylvain Corlay         *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#include <complex>
#include <cstddef>
#include <vector>

#include "test_common.hpp"

#include "xtensor-r/rtensor.hpp"
#include "xtensor-r/rvectorize.hpp"

namespace xt
{
    double f1(double a, double b)
    {
        return a + b;
    }

    using shape_type = std::vector<std::size_t>;

    TEST(rvectorize, function)
    {
        auto vecf1 = rvectorize(f1);
        shape_type shape = { 3, 2 };
        rarray<double> a(shape, 1.5);
        rarray<double> b(shape, 2.3);
        rarray<double> c = vecf1(a, b);
        EXPECT_EQ(a(0, 0) + b(0, 0), c(0, 0));
    }

    TEST(rvectorize, lambda)
    {
        auto vecf1 = rvectorize([](double a, double b) { return a + b; });
        shape_type shape = { 3, 2 };
        rarray<double> a(shape, 1.5);
        rarray<double> b(shape, 2.3);
        rarray<double> c = vecf1(a, b);
        EXPECT_EQ(a(0, 0) + b(0, 0), c(0, 0));
    }

    TEST(rvectorize, complex)
    {
        using complex_t = std::complex<double>;
        shape_type shape = { 3, 2 };
        rarray<complex_t> a(shape, complex_t(1.2, 2.5));
        auto f = rvectorize([](complex_t x) { return std::abs(x); });
        auto res = f(a);
        double exp = std::abs(a(1, 1));
        EXPECT_EQ(exp, res(1, 1));
    }
}
