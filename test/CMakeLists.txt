############################################################################
# Copyright (c) 2016, Wolf Vollprecht, Johan Mabille and Sylvain Corlay    #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

cmake_minimum_required(VERSION 3.1)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(xtensor-r-test)

    find_package(xtensor REQUIRED CONFIG)
    set(XTENSOR_INCLUDE_DIR ${xtensor_INCLUDE_DIRS})

    find_package(xtensor-r REQUIRED CONFIG)
    set(XTENSOR_R_INCLUDE_DIR ${xtensor-r_INCLUDE_DIRS})
endif ()

message(STATUS "Forcing tests build type to Release")
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

include(CheckCXXCompilerFlag)

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wunused-parameter -Wextra -Wreorder -Wconversion")
    CHECK_CXX_COMPILER_FLAG("-std=c++14" HAS_CPP14_FLAG)

    if (HAS_CPP14_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    else()
        message(FATAL_ERROR "Unsupported compiler -- xtensor requires C++14 support!")
    endif()
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /MP /bigobj")
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

if (DOWNLOAD_GTEST OR GTEST_SRC_DIR)
    if(DOWNLOAD_GTEST)
        # Download and unpack googletest at configure time
        configure_file(downloadGTest.cmake.in googletest-download/CMakeLists.txt)
    else()
        # Copy local source of googletest at configure time
        configure_file(copyGTest.cmake.in googletest-download/CMakeLists.txt)
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()

    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                     ${CMAKE_CURRENT_BINARY_DIR}/googletest-build)

    set(GTEST_INCLUDE_DIRS "${gtest_SOURCE_DIR}/include")
    set(GTEST_BOTH_LIBRARIES gtest_main gtest)
else()
    find_package(GTest REQUIRED)
endif()

find_package(Threads)

include_directories(${GTEST_INCLUDE_DIRS})

find_package(R REQUIRED)

include_directories(${R_INCLUDE_DIR} SYSTEM)

link_directories(${R_ROOT_DIR}/lib)
link_directories(${R_ROOT_DIR}/bin/${R_LIB_ARCH})
link_directories(${RINSIDE_LOCATION_LINE}/libs/${R_LIB_ARCH})
link_directories(${RINSIDE_LOCATION_LINE}/lib)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

set(XTENSOR_R_TESTS
    main.cpp
    test_rarray.cpp
    test_rtensor.cpp
    test_rvectorize.cpp
)
set(XTENSOR_R_TARGET test_xtensor_r)
add_executable(${XTENSOR_R_TARGET} ${XTENSOR_R_TESTS} ${XTENSOR_R_HEADERS})
target_link_libraries(${XTENSOR_R_TARGET} xtensor-r ${R_LIBRARIES} ${RINSIDE_LIBRARIES} ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

if(DOWNLOAD_GTEST OR GTEST_SRC_DIR)
    add_dependencies(${XTENSOR_R_TARGET} gtest_main)
endif()

if(UNIX)
    add_custom_target(xtest COMMAND export R_HOME=${R_HOME} && ./test_xtensor_r DEPENDS ${XTENSOR_R_TARGET})
else()
    add_custom_target(xtest COMMAND set R_HOME=${R_HOME} && .\\test_xtensor_r DEPENDS ${XTENSOR_R_TARGET})
endif()
