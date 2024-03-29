/***************************************************************************
* Copyright (c) Wolf Vollprecht, Johan Mabille and Sylvain Corlay          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <limits>

#include "gtest/gtest.h"
#include "xtensor-r/rtensor.hpp"
#include "xtensor-r/rarray.hpp"
#include "xtensor/xarray.hpp"
#include "xtensor/xtensor.hpp"

namespace xt
{
    template <class E, std::enable_if_t<!xt::has_fixed_rank_t<E>::value, int> = 0>
    inline bool sfinae_has_fixed_rank(E&&)
    {
        return false;
    }

    template <class E, std::enable_if_t<xt::has_fixed_rank_t<E>::value, int> = 0>
    inline bool sfinae_has_fixed_rank(E&&)
    {
        return true;
    }

    TEST(sfinae, fixed_rank)
    {
        xt::rarray<int> a = {{9, 9, 9}, {9, 9, 9}};
        xt::rtensor<int, 1> b = {9, 9};
        xt::rtensor<int, 2> c = {{9, 9}, {9, 9}};

        EXPECT_TRUE(sfinae_has_fixed_rank(a) == false);
        EXPECT_TRUE(sfinae_has_fixed_rank(b) == true);
        EXPECT_TRUE(sfinae_has_fixed_rank(c) == true);
    }

    TEST(sfinae, get_rank)
    {
        xt::rtensor<double, 1> A = xt::zeros<double>({2});
        xt::rtensor<double, 2> B = xt::zeros<double>({2, 2});
        xt::rarray<double> C = xt::zeros<double>({2, 2});

        EXPECT_TRUE(xt::get_rank<decltype(A)>::value == 1ul);
        EXPECT_TRUE(xt::get_rank<decltype(B)>::value == 2ul);
        EXPECT_TRUE(xt::get_rank<decltype(C)>::value == SIZE_MAX);
    }
}
