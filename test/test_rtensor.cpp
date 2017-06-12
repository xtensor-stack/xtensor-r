/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"
#include "test_common.hpp"
#include "xtensor-r/rtensor.hpp"
#include "xtensor/xtensor.hpp"

namespace xt
{
    using container_type = std::vector<std::size_t>;

    TEST(rtensor, initializer_constructor)
    {
        rtensor<int, 3> t 
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

    TEST(rtensor, shaped_constructor)
    {
        {
            SCOPED_TRACE("column_major constructor");
            column_major_result<container_type> cm;
            rtensor<int, 3> ca(cm.m_shape);
            compare_shape(ca, cm);
            EXPECT_EQ(layout_type::column_major, ca.layout());
        }
    }

    TEST(rtensor, strided_constructor)
    {
        central_major_result<container_type> cmr;
        rtensor<int, 3> cma(cmr.m_shape);
        compare_shape(cma, cmr);
    }

    TEST(rtensor, valued_constructor)
    {
        {
            SCOPED_TRACE("column_major valued constructor");
            column_major_result<container_type> cm;
            int value = 2;
            rtensor<int, 3> ca(cm.m_shape, value);
            compare_shape(ca, cm);
            std::vector<int> vec(ca.size(), value);
            EXPECT_TRUE(std::equal(vec.begin(), vec.end(), ca.data().begin()));
        }
    }

    TEST(rtensor, strided_valued_constructor)
    {
        column_major_result<container_type> cmr;
        int value = 2;
        rtensor<int, 3> cma(cmr.m_shape, value);
        compare_shape(cma, cmr);
        std::vector<int> vec(cma.size(), value);
        EXPECT_TRUE(std::equal(vec.begin(), vec.end(), cma.data().begin()));
    }

    TEST(rtensor, copy_semantic)
    {
        column_major_result<container_type> res;
        int value = 2;
        rtensor<int, 3> a(res.m_shape, value);

        {
            SCOPED_TRACE("copy constructor");
            rtensor<int, 3> b(a);
            compare_shape(a, b);
            EXPECT_EQ(a.data(), b.data());
            a.data()[0] += 1;
            EXPECT_NE(a.data()[0], b.data()[0]);
        }

        {
            SCOPED_TRACE("assignment operator");
            column_major_result<container_type> r;
            rtensor<int, 3> c(r.m_shape, 0);
            EXPECT_NE(a.data(), c.data());
            c = a;
            compare_shape(a, c);
            EXPECT_EQ(a.data(), c.data());
            a.data()[0] += 1;
            EXPECT_NE(a.data()[0], c.data()[0]);
        }
    }

    TEST(rtensor, move_semantic)
    {
        column_major_result<container_type> res;
        int value = 2;
        rtensor<int, 3> a(res.m_shape, value);

        {
            SCOPED_TRACE("move constructor");
            rtensor<int, 3> tmp(a);
            rtensor<int, 3> b(std::move(tmp));
            compare_shape(a, b);
            EXPECT_EQ(a.data(), b.data());
        }

        {
            SCOPED_TRACE("move assignment");
            column_major_result<container_type> r;
            rtensor<int, 3> c(r.m_shape, 0);
            EXPECT_NE(a.data(), c.data());
            rtensor<int, 3> tmp(a);
            c = std::move(tmp);
            compare_shape(a, c);
            EXPECT_EQ(a.data(), c.data());
        }
    }

    TEST(rtensor, extended_constructor)
    {
        xt::xtensor<int, 2> a1 = { {1, 2}, {3, 4} };
        xt::xtensor<int, 2> a2 = { {1, 2}, {3, 4} };
        rtensor<int, 2> c = a1 + a2;
        EXPECT_EQ(c(0, 0), a1(0, 0) + a2(0, 0));
        EXPECT_EQ(c(0, 1), a1(0, 1) + a2(0, 1));
        EXPECT_EQ(c(1, 0), a1(1, 0) + a2(1, 0));
        EXPECT_EQ(c(1, 1), a1(1, 1) + a2(1, 1));
    }

    TEST(rtensor, reshape)
    {
        rtensor<int, 3> a;
        test_reshape<rtensor<int, 3>, container_type>(a);
    }

    /*TEST(rtensor, transpose)
    {
        rtensor<int, 3> a;
        test_transpose<rtensor<int, 3>, container_type>(a);
    }*/

    TEST(rtensor, access)
    {
        rtensor<int, 3> a;
        test_access<rtensor<int, 3>, container_type>(a);
    }

    TEST(rtensor, indexed_access)
    {
        rtensor<int, 3> a;
        test_indexed_access<rtensor<int, 3>, container_type>(a);
    }

    TEST(rtensor, broadcast_shape)
    {
        rtensor<int, 4> a;
        test_broadcast(a);
    }

    TEST(rtensor, iterator)
    {
        rtensor<int, 3> a;
        test_iterator<rtensor<int, 3>, container_type>(a);
    }

    TEST(rtensor, zerod)
    {
        rtensor<int, 3> a;
        EXPECT_EQ(0, a());
    }

}
