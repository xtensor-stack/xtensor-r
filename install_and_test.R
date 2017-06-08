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

Rcpp::compileAttributes('xtensor-r')

build('xtensor-r')

# Compiling the package

Sys.setenv("PKG_CXXFLAGS"="-std=c++14")
install.packages('xtensor_0.1.0.tar.gz')

# Testing

A <- array(seq(2 * 3 * 4 * 5), c(2, 3, 4, 5))
A[ , , , 1] = 123
print(A)
result = xtensor::rcpp_hello_xtensor(A)

cleanMem <- function(n=10) { for (i in 1:n) gc() }
cleanMem()

print(result)

print(A)