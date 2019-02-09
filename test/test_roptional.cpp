/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#include "xtensor-r/roptional.hpp"

namespace xt
{
    TEST(roptional, roptional_access)
    {
        using cpp_type = double;
        constexpr static int rtype = Rcpp::traits::r_sexptype_traits<cpp_type>::rtype;
        auto mi = Rcpp::traits::get_na<rtype>();

        rarray<cpp_type> t
          {{{ 0,  1,  2},
            { 3, mi, 5},
            { 6, mi, 8}},
           {{ 9, 10, 11},
            {mi, mi, mi},
            {15, 16, 17}}};
        SEXP exp = SEXP(t);
        rarray_optional<cpp_type> o(exp);

        // Check has_value on elements
        EXPECT_TRUE(o(0, 0, 0).has_value());
        EXPECT_TRUE(o(0, 0, 1).has_value());
        EXPECT_TRUE(o(0, 0, 2).has_value());

        EXPECT_TRUE(o(0, 1, 0).has_value());
        EXPECT_FALSE(o(0, 1, 1).has_value());
        EXPECT_TRUE(o(0, 1, 2).has_value());

        EXPECT_TRUE(o(0, 2, 0).has_value());
        EXPECT_FALSE(o(0, 2, 1).has_value());
        EXPECT_TRUE(o(0, 2, 2).has_value());

        EXPECT_TRUE(o(1, 0, 0).has_value());
        EXPECT_TRUE(o(1, 0, 1).has_value());
        EXPECT_TRUE(o(1, 0, 2).has_value());

        EXPECT_FALSE(o(1, 1, 0).has_value());
        EXPECT_FALSE(o(1, 1, 1).has_value());
        EXPECT_FALSE(o(1, 1, 2).has_value());

        EXPECT_TRUE(o(1, 2, 0).has_value());
        EXPECT_TRUE(o(1, 2, 1).has_value());
        EXPECT_TRUE(o(1, 2, 2).has_value());

        // Check has_value() on expression
        auto hv = o.has_value();
        EXPECT_TRUE(hv(0, 2, 0));
        EXPECT_FALSE(hv(0, 2, 1));
        EXPECT_TRUE(hv(0, 2, 2));

        // Check value() on expression
        auto v = o.value();
        EXPECT_EQ(v(0, 2, 0), 6.0);
        EXPECT_EQ(v(0, 2, 2), 8.0);
    }

    TEST(roptional, roptional_set)
    {
        using cpp_type = double;
        constexpr static int rtype = Rcpp::traits::r_sexptype_traits<cpp_type>::rtype;
        auto mi = Rcpp::traits::get_na<rtype>();

        rarray<cpp_type> t
          {{{ 0,  1,  2},
            { 3, mi, 5},
            { 6, mi, 8}},
           {{ 9, 10, 11},
            {mi, mi, mi},
            {15, 16, 17}}};
        SEXP exp = SEXP(t);
        rarray_optional<cpp_type> o(exp);

        // Test setting element
        EXPECT_FALSE(o(0, 2, 1).has_value());
        o(0, 2, 1) = 1.0;
        EXPECT_TRUE(o(0, 2, 1).has_value());
        o(0, 2, 1).has_value() = false;
        EXPECT_FALSE(o(0, 2, 1).has_value());

        // Test setting xtl::missing
        EXPECT_TRUE(o(0, 0, 0).has_value());
        o(0, 0, 0) = xtl::missing<double>();
        EXPECT_FALSE(o(0, 0, 0).has_value());
    }

    TEST(roptional, assign_optional)
    {
        using cpp_type = double;
        constexpr static int rtype = Rcpp::traits::r_sexptype_traits<cpp_type>::rtype;
        auto mi = Rcpp::traits::get_na<rtype>();

        rarray<cpp_type> t {0};
        SEXP exp = SEXP(t);
        rarray_optional<cpp_type> o(exp);

        /*

        // Uncomment upon release of xtensor 0.19.3
        // Adding strides() method to xfunctor_adaptor.

        // Assign expression
        xtensor_optional<double, 2> m
            {{ 1.0 ,       2.0         },
             { 3.0 , xtl::missing<double>() }};
        o = m;

        EXPECT_EQ(o.dimension(), 2);
        EXPECT_TRUE(o(0, 0).has_value());
        EXPECT_TRUE(o(0, 1).has_value());
        EXPECT_TRUE(o(1, 0).has_value());
        EXPECT_FALSE(o(1, 1).has_value());
        
        */
    }
}
