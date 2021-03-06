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

R encodes missing values (NAs) as special values of the underlying type.

Xtensor-r supports R's missing values with the ``rarray_optional`` and
``rtensor_optional`` counterparts to ``rarray`` and ``rtensor`` which enable
operations with missing values with the ``xoptional`` API.

R defining PI as macro
----------------------

Ancient versions of ``S`` used to define a ``PI`` macro. This macro collides with
xtensor, as xtensor is using ``PI`` as a variable name in the numeric constants.

If you're encountering this issue, either reorder your headers so that the xmath
header of xtensor is included before Rcpp / xtensor-r or use the following define
before including xtensor-r and Rcpp:

.. code::

    #define STRICT_R_HEADERS

This prevents the PI macro definition.
