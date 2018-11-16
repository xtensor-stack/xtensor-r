.. Copyright (c) 2016, Wolf Vollprecht, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

.. _r-pec:

R Peculiarities
===============

R supports only a subset of types available in C++. The types which are natively
supported with xtensor-R are int (32 bit), double, complex, byte.
Using other types (such as a C++ ``float``) in a  ``rarray`` or ``rtensor`` will fail to compile.

Below is a mapping from R to xtensor:

+----------------+----------------------------------+
| R type         | xtensor type                     |
+================+==================================+
| Integer Vector | rarray<int> or rarray<int32_t>   |
+----------------+----------------------------------+
| Real Vector    | rarray<double>                   |
+----------------+----------------------------------+
| Complex Vector | rarray<std::complex<double>>     |
+----------------+----------------------------------+
| Raw Vector     | rarray<Rbyte> or rarray<uint8_t> |
+----------------+----------------------------------+
| Logical Vector | Not supported at the moment      |
+----------------+----------------------------------+

Note that, for illustration purposes, ``rarray`` was used above. The ``rtensor``
container works just as well.

R's NA and NaN values
---------------------

R supports NaN and NA for all types (except the Raw Vector). xtensor does not yet
support NaNs for numbers other than double, and does not support NA (missing value)
for any R type. We have support for missing values in the core xtensor library
and we hope to add support for R's missing values in the future.