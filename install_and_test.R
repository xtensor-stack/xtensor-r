############################################################################
# Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

# If this doesn't work, please install Rcpp and 
# devtools:
# 
# install.packages('Rcpp')
# install.packages('devtools')

library('Rcpp')
library('devtools')
setwd('..')

Rcpp::compileAttributes('xtensor')

build('xtensor')

# Compiling the package

install.packages('xtensor_0.1.0.tar.gz')

# Testing

A <- array(seq(2), c(2, 1, 1, 1))
# A[ , , , 1] = 123
# print(A)
b <- array(32.23, c(4, 2, 2))
xtensor::rcpp_hello_xtensor(4:123)
