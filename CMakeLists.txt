############################################################################
# Copyright (c) 2016, Wolf Vollprechet, Johan Mabille and Sylvain Corlay   #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

cmake_minimum_required(VERSION 3.1)
project(xtensor-r)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(XTENSOR_R_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Versionning
# ===========

set(XTENSOR_R_CONFIG_FILE
    "${XTENSOR_R_INCLUDE_DIR}/xtensor-r/xtensor_r_config.hpp")
file(STRINGS ${XTENSOR_R_CONFIG_FILE} xtensor_python_version_defines
    REGEX "#define XTENSOR_R_VERSION_(MAJOR|MINOR|PATCH)")
foreach(ver ${xtensor_python_version_defines})
    if(ver MATCHES "#define XTENSOR_R_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(XTENSOR_R_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif()
endforeach()
set(${PROJECT_NAME}_VERSION
    ${XTENSOR_R_VERSION_MAJOR}.${XTENSOR_R_VERSION_MINOR}.${XTENSOR_R_VERSION_PATCH})
message(STATUS "xtensor-r v${${PROJECT_NAME}_VERSION}")

# Dependencies
# ============

find_package(xtl 0.4.16 REQUIRED)
message(STATUS "Found xtl: ${xtl_INCLUDE_DIRS}/xtl")
find_package(xtensor 0.18.1 REQUIRED)
message(STATUS "Found xtensor: ${xtensor_INCLUDE_DIRS}/xtensor")

# Build
# =====

set(XTENSOR_R_HEADERS
    ${XTENSOR_R_INCLUDE_DIR}/xtensor-r/rarray.hpp
    ${XTENSOR_R_INCLUDE_DIR}/xtensor-r/rcontainer.hpp
    ${XTENSOR_R_INCLUDE_DIR}/xtensor-r/rtensor.hpp
    ${XTENSOR_R_INCLUDE_DIR}/xtensor-r/rvectorize.hpp
    ${XTENSOR_R_INCLUDE_DIR}/xtensor-r/xtensor_r_config.hpp
)

add_library(xtensor-r INTERFACE)
target_include_directories(xtensor-r INTERFACE $<BUILD_INTERFACE:${XTENSOR_R_INCLUDE_DIR}>
                                               $<INSTALL_INTERFACE:include>)

target_link_libraries(xtensor-r INTERFACE xtensor)
get_target_property(inc_dir xtensor-r INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "${inc_dir}")

OPTION(BUILD_TESTS "xtensor-r test suite" OFF)
OPTION(DOWNLOAD_GTEST "build gtest from downloaded sources" OFF)

if(DOWNLOAD_GTEST OR GTEST_SRC_DIR)
    set(BUILD_TESTS ON)
endif()

find_package(R REQUIRED)

if(BUILD_TESTS)
    # Locate Rcpp by invoking R
    execute_process(
        COMMAND ${R_COMMAND} -q -e "find.package('Rcpp')"
        OUTPUT_VARIABLE RCPP_FIND_RESULT_STRING
        RESULT_VARIABLE RCPP_FIND_RESULT)
    string(REGEX MATCH "\".+\"" RCPP_LOCATION_LINE ${RCPP_FIND_RESULT_STRING})
    string(REGEX REPLACE "\"" "" RCPP_LOCATION_LINE ${RCPP_LOCATION_LINE})
    string(CONCAT Rcpp_INCLUDE_DIRS ${RCPP_LOCATION_LINE} "/include/")
    message("RCpp Include Directory: " ${Rcpp_INCLUDE_DIRS})
    include_directories(${Rcpp_INCLUDE_DIRS})

    # Locate RInside by invoking R
    execute_process(
        COMMAND ${R_COMMAND} -q -e "find.package('RInside')"
        OUTPUT_VARIABLE RINSIDE_FIND_RESULT_STRING
        RESULT_VARIABLE RINSIDE_FIND_RESULT)
    string(REGEX MATCH "\".+\"" RINSIDE_LOCATION_LINE ${RINSIDE_FIND_RESULT_STRING})
    string(REGEX REPLACE "\"" "" RINSIDE_LOCATION_LINE ${RINSIDE_LOCATION_LINE})
    string(CONCAT RInside_INCLUDE_DIRS ${RINSIDE_LOCATION_LINE} "/include/")
    message("RInside Include Directory: " ${RInside_INCLUDE_DIRS})

    set(OLD_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES} ".dll")
    find_library(RINSIDE_LIBRARIES
        NAMES RInside
        HINTS ${RINSIDE_LOCATION_LINE}/lib ${RINSIDE_LOCATION_LINE}/libs/${R_LIB_ARCH}
    )
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${OLD_SUFFIXES})
    include_directories(${RInside_INCLUDE_DIRS})

    # Include the tests
    add_subdirectory(test)
endif()

# C++ Module Installation
# =======================

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS xtensor-r
        EXPORT ${PROJECT_NAME}-targets)

# Makes the project importable from the build directory
export(EXPORT ${PROJECT_NAME}-targets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

install(FILES ${XTENSOR_R_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xtensor-r)

set(XTENSOR_R_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE
    STRING "install path for xtensor-rConfig.cmake")

configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${XTENSOR_R_CMAKECONFIG_INSTALL_DIR})

# xtensor-r is header-only and does not depend on the architecture.
# Remove CMAKE_SIZEOF_VOID_P from xtensor-rConfigVersion.cmake so that an xtensor-rConfig.cmake
# generated for a 64 bit target can be used for 32 bit targets and vice versa.
set(_XTENSOR_CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})
unset(CMAKE_SIZEOF_VOID_P)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
                                 VERSION ${${PROJECT_NAME}_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
set(CMAKE_SIZEOF_VOID_P ${_XTENSOR_CMAKE_SIZEOF_VOID_P})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${XTENSOR_R_CMAKECONFIG_INSTALL_DIR})
install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}Targets.cmake
        DESTINATION ${XTENSOR_R_CMAKECONFIG_INSTALL_DIR})

# R Package Target
# ================

file(COPY ${CMAKE_SOURCE_DIR}/R-package DESTINATION ${CMAKE_BINARY_DIR})

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/R-package/inst)

set(XTENSOR_TAG 0.18.1)
include(ExternalProject)
ExternalProject_Add(xtensor-clone
    GIT_REPOSITORY https://github.com/QuantStack/xtensor
    GIT_TAG ${XTENSOR_TAG}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION} -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
)
set_target_properties(xtensor-clone PROPERTIES EXCLUDE_FROM_ALL TRUE)

set(XTL_TAG 0.4.16)
ExternalProject_Add(xtl-clone
    GIT_REPOSITORY https://github.com/QuantStack/xtl
    GIT_TAG ${XTL_TAG}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION} -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
)
set_target_properties(xtl-clone PROPERTIES EXCLUDE_FROM_ALL TRUE)

ExternalProject_Add(xtensor-r-clone
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION} -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
)
set_target_properties(xtensor-r-clone PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_custom_target(cran DEPENDS xtensor-clone xtl-clone xtensor-r-clone)

OPTION(XTENSOR_INSTALL_R_PACKAGES "Install Rcpp and devtools from CRAN" ON)

if (XTENSOR_INSTALL_R_PACKAGES)
    # Install Rcpp and devtools
    add_custom_command(
        TARGET cran
        POST_BUILD
        COMMAND ${R_COMMAND} -q -e \"install.packages\(c\(\'Rcpp\',\'devtools\'\),repos=\'http://cran.us.r-project.org\'\)\"
    )
endif()

# Run Rcpp compileAttributes
add_custom_command(
    TARGET cran
    POST_BUILD
    COMMAND ${R_COMMAND} -q -e \"library\(\'Rcpp\'\)\;Rcpp::compileAttributes\(\'R-package\'\)\;\"
)

# Run devtools build
add_custom_command(
    TARGET cran
    POST_BUILD
    COMMAND ${R_COMMAND} -q -e \"library\(\'devtools\'\)\;build\(\'R-package\'\)\;\"
)
