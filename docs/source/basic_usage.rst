.. Copyright (c) 2016, Wolf Vollprecht, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Basic Usage
===========

Example : Use an algorithm of the C++ library on a R array inplace
------------------------------------------------------------------

**C++ code**

.. code::

    #include <numeric>                    // Standard library import for std::accumulate
    #define STRICT_R_HEADERS              // Otherwise a PI macro is defined in R
    #include "xtensor/xmath.hpp"          // xtensor import for the C++ universal functions
    #include "xtensor-r/rarray.hpp"       // R bindings
    #include <Rcpp.h>

    using namespace Rcpp;

    // [[Rcpp::plugins(cpp14)]]

    // [[Rcpp::export]]
    double sum_of_sines(xt::rarray<double>& m)
    {
        auto sines = xt::sin(m);  // sines does not actually hold values.
        return std::accumulate(sines.cbegin(), sines.cend(), 0.0);
    }

**R code:**

.. code::

    v <- matrix(0:14, nrow=3, ncol=5)
    s <- sum_of_sines(v)
    s

**Outputs**

.. code::

    1.2853996391883833

Note that R places some restriction on what C++ types are usable. We've noted the
differences in :ref:`r-pec`.