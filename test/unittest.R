# install.packages('testthat', repos='http://cran.us.r-project.org')

library(testthat)

Rcpp::sourceCpp("rcpp_tests.cpp", cacheDir="__cache__")

x <- array(as.numeric(1:100), dim=c(10, 10))

test_that("cpp_inplace", {
	expect_equal(x[1, 1], 1)
	expect_equal(x[10, 3], 30)
	expect_equal(modify_cpp(x), 1)
	expect_equal(x[2, 3], 22)
	expect_equal(x[1, 1], -1000)
	expect_equal(x[10, 3], 1000)
})

x <- array(as.numeric(1:100), dim=c(10, 10))
y <- array(as.numeric(1:100), dim=c(10, 10))

test_that("cpp_add", {
	expect_equal(cpp_add(x, y), x + y)
})

x <- array(as.numeric(1:30), dim=c(1, 10, 3))

test_that("cpp_reshape", {
	expect_equal(dim(x), c(1, 10, 3))
	reshape_cpp(x)
	expect_equal(dim(x), c(3, 10))
})

x <- array(1:10, c(2, 5))

test_that("call_int", {
	call_int(x)
	expect_equal(x[2, 2], 35)
})

x <- array(TRUE, c(2, 5))

test_that("call_lgl", {
  call_lgl(x)
  expect_equal(x[2, 2], FALSE)
})

x <- array(as.complex(1:10), c(2, 5))

x[1, 1] <- 0 + 1i
x[2, 3] <- 1 + 5i

test_that("call_stdcomplex", {
	call_stdcomplex(x)
	expect_equal(x[1, 3], -10+-100i)
})

test_that("reduce_1d", {
	res1d <- reduce_1d()
	expect_equal(res1d, 10)
})