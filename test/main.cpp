/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtensor-r/rarray.hpp"

#include "gtest/gtest.h"
#include "R.h"

extern "C" {
   int Rf_initEmbeddedR(int argc, char**argv);
   void Rf_endEmbeddedR(int fatal);
   int R_running_as_main_program;
}

#include <iostream>

int main(int argc, char* argv[])
{
    R_running_as_main_program = 1;
    Rf_initEmbeddedR(argc, argv);
    // Initialize all the things (google-test and Python interpreter)
    ::testing::InitGoogleTest(&argc, argv);

    // Run test suite
    int ret = RUN_ALL_TESTS();

    Rf_endEmbeddedR(0);

    return ret;
}

