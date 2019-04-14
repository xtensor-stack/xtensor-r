/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#include "xtensor/xarray.hpp"

#include "xtensor-r/rarray.hpp"

#include "test_common.hpp"

namespace xt
{
    TEST(rarray, initializer_constructor)
    {
        rarray<int> t 
          {{{ 0,  1,  2}, 
            { 3,  4,  5}, 
            { 6,  7,  8}}, 
           {{ 9, 10, 11}, 
            {12, 13, 14}, 
            {15, 16, 17}}}; 

        EXPECT_EQ(t.dimension(), 3);
        EXPECT_EQ(t(0, 0, 1), 1);
        EXPECT_EQ(t.shape()[0], 2);
    }

    TEST(rarray, shaped_constructor)
    {
        {
            SCOPED_TRACE("column_major constructor");
            column_major_result<> cm;
            rarray<int> ca(cm.m_shape);
            compare_shape(ca, cm);
            EXPECT_EQ(layout_type::column_major, ca.layout());
        }
    }

    TEST(rarray, valued_constructor)
    {
        {
            SCOPED_TRACE("column_major valued constructor");
            column_major_result<> cm;
            int value = 2;
            rarray<int> ca(cm.m_shape, value);
            compare_shape(ca, cm);
            std::vector<int> vec(ca.size(), value);
            EXPECT_TRUE(std::equal(vec.cbegin(), vec.cend(), ca.storage().cbegin()));
        }
    }

    TEST(rarray, copy_semantic)
    {
        column_major_result<> res;
        int value = 2;
        rarray<int> a(res.m_shape, value);

        {
            SCOPED_TRACE("copy constructor");
            rarray<int> b(a);
            compare_shape(a, b);
            EXPECT_EQ(a.storage(), b.storage());
            a.storage()[0] += 1;
            EXPECT_NE(a.storage()[0], b.storage()[0]);
        }

        {
            SCOPED_TRACE("assignment operator");
            column_major_result<> r;
            rarray<int> c(r.m_shape, 0);
            EXPECT_NE(a.storage(), c.storage());
            c = a;
            compare_shape(a, c);
            EXPECT_EQ(a.storage(), c.storage());
            a.storage()[0] += 1;
            EXPECT_NE(a.storage()[0], c.storage()[0]);
        }
    }

    TEST(rarray, move_semantic)
    {
        column_major_result<> res;
        int value = 2;
        rarray<int> a(res.m_shape, value);

        {
            SCOPED_TRACE("move constructor");
            rarray<int> tmp(a);
            rarray<int> b(std::move(tmp));
            compare_shape(a, b);
            EXPECT_EQ(a.storage(), b.storage());
        }

        {
            SCOPED_TRACE("move assignment");
            column_major_result<> r;
            rarray<int> c(r.m_shape, 0);
            EXPECT_NE(a.storage(), c.storage());
            rarray<int> tmp(a);
            c = std::move(tmp);
            compare_shape(a, c);
            EXPECT_EQ(a.storage(), c.storage());
        }
    }

    TEST(rarray, extended_constructor)
    {
        xt::xarray<int> a1 = { { 1, 2 },{ 3, 4 } };
        xt::xarray<int> a2 = { { 1, 2 },{ 3, 4 } };
        rarray<int> c = a1 + a2;
        EXPECT_EQ(c(0, 0), a1(0, 0) + a2(0, 0));
        EXPECT_EQ(c(0, 1), a1(0, 1) + a2(0, 1));
        EXPECT_EQ(c(1, 0), a1(1, 0) + a2(1, 0));
        EXPECT_EQ(c(1, 1), a1(1, 1) + a2(1, 1));
    }

    TEST(rarray, resize)
    {
        rarray<int> a;
        test_resize(a);
    }

    TEST(rarray, access)
    {
        rarray<int> a;
        test_access(a);
    }

    TEST(rarray, indexed_access)
    {
        rarray<int> a;
        test_indexed_access(a);
    }

    TEST(rarray, broadcast_shape)
    {
        rarray<int> a;
        test_broadcast(a);
        test_broadcast2(a);
    }

    TEST(rarray, iterator)
    {
        rarray<int> a;
        test_iterator(a);
    }

    TEST(rarray, initializer_list)
    {
        // rarray<int> a0(1);
        rarray<int> a1({1, 2});
        rarray<int> a2({{1, 2}, {2, 4}, {5, 6}});
        // EXPECT_EQ(1, a0());
        EXPECT_EQ(2, a1(1));
        EXPECT_EQ(4, a2(1, 1));
    }

    // TODO zerod
    // TEST(rarray, zerod)
    // {
    //     rarray<int> a;
    //     EXPECT_EQ(0, a());
    // }

    TEST(rarray, reshape)
    {
        rarray<int> a = {{1,2,3}, {4,5,6}};
        auto ptr = a.data();
        a.reshape({1, 6});
        std::vector<std::size_t> sc1({1, 6});
        EXPECT_TRUE(std::equal(sc1.cbegin(), sc1.cend(), a.shape().cbegin()) && a.shape().size() == 2);
        EXPECT_EQ(ptr, a.data());
        a.reshape({6});
        std::vector<std::size_t> sc2 = {6};
        EXPECT_TRUE(std::equal(sc2.cbegin(), sc2.cend(), a.shape().cbegin()) && a.shape().size() == 1);
        EXPECT_EQ(ptr, a.data());
    }

    TEST(rarray, from_shape)
    {
        auto a = rarray<int>::from_shape({4, 5});
        EXPECT_EQ(a.shape()[0], 4);
        EXPECT_EQ(a.shape()[1], 5);
        EXPECT_EQ(a.size(), 20);
    }
}
