/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>

#include "gtest/gtest.h"
#include "xtensor-r/rarray.hpp"

#include "RInside.h"

int main(int argc, char* argv[])
{

    RInside R(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);

    // Run test suite
    int ret = RUN_ALL_TESTS();

    exit(ret);
}

