/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#include "xtensor/xarray.hpp"
#include "xtensor/xmath.hpp"

#include "xtensor-r/rarray.hpp"

namespace xt
{
    TEST(rxreducer, sum)
    {
        xt::rarray<int> x_array = {{1, 2, 3}, {4, 5, 6}};
        auto x_sum = xt::sum(x_array, 1);

        EXPECT_EQ(x_sum(0), 6);
        EXPECT_EQ(x_sum(1), 15);
    }
}
