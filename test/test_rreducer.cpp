/***************************************************************************
* Copyright (c) Wolf Vollprecht, Johan Mabille, and Sylvain Corlay         *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <gtest/gtest.h>

#include <xtensor/xtensor.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xmath.hpp>

#include "xtensor-r/rarray.hpp"
#include "xtensor-r/rtensor.hpp"

namespace xt
{
    TEST(rxreducer, sum)
    {
        xt::rarray<int> x_array = {{1, 2, 3}, {4, 5, 6}};
        auto x_sum = xt::sum(x_array, 1);

        EXPECT_EQ(x_sum(0), 6);
        EXPECT_EQ(x_sum(1), 15);
    }

#ifndef __clang__
    TEST(rxreducer, variance)
    {
        // For computing references
        xt::xtensor<int, 2> data = {{1, 2, 3}, {4, 5, 6}};
        std::vector<std::size_t> const axes = {1};

        {  // xt:rarray
            xt::rarray<int> arr = data;
            EXPECT_EQ(xt::variance(arr)(0), xt::variance(data)(0));
            auto var = xt::variance(arr, axes);
            EXPECT_EQ(xt::variance(arr, axes), xt::variance(data, axes));
        }
        {  // xt:rtensor
            xt::rtensor<int, 2> arr = data;
            EXPECT_EQ(xt::variance(arr)(0), xt::variance(data)(0));
            auto var = xt::variance(arr, axes);
            EXPECT_EQ(xt::variance(arr, axes), xt::variance(data, axes));
        }
    }
#endif
}
