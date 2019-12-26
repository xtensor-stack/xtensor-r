.. Copyright (c) 2016, Wolf Vollprecht, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

.. image:: xtensor-r.svg

R bindings for the xtensor_ C++ multi-dimensional array library.

Introduction
------------

What are ``xtensor`` and ``xtensor-r``?

 - ``xtensor`` is a C++ library for multi-dimensional arrays enabling numpy-style broadcasting and lazy computing.
 - ``xtensor-r`` enables inplace use of R arrays with all the benefits from ``xtensor``

     - C++ universal functions and broadcasting 
     - STL - compliant APIs.

The `numpy to xtensor cheat sheet`_ from the ``xtensor`` documentation shows how numpy APIs translate to C++ with ``xtensor``.

The Python bindings for ``xtensor`` are based on the Rcpp_ C++ library, which enables seemless interoperability between C++ and Python.

Enabling R arrays in your C++ libraries
---------------------------------------

Instead of exposing new types to R, ``xtensor-r`` enables the use of R data structures from C++ using R's C API.

In addition to the basic accessors and iterators of ``xtensor`` containers, it also enables using R arrays with ``xtensor``'s expression system.

Besides ``xtensor-r`` provides an API to create *Universal functions* from simple scalar functions from your C++ code.

``xtensor`` and ``xtensor-r`` require a modern C++ compiler supporting C++14. The following C++ compilers are supported:

- On Windows platforms, Visual C++ 2015 Update 2, or more recent
- On Unix platforms, gcc 4.9 or a recent version of Clang

Licensing
---------

We use a shared copyright model that enables all contributors to maintain the
copyright on their contributions.

This software is licensed under the BSD-3-Clause license. See the LICENSE file for details.

.. toctree::
   :caption: INSTALLATION
   :maxdepth: 2

   installation


.. toctree::
   :caption: USAGE
   :maxdepth: 2

   basic_usage
   array_tensor
   r_peculiarities

.. toctree::
   :caption: API REFERENCE
   :maxdepth: 2

   api_reference

.. toctree::
   :caption: DEVELOPER ZONE

   releasing

.. _`numpy to xtensor cheat sheet`: http://xtensor.readthedocs.io/en/latest/numpy.html 
.. _xtensor: https://github.com/xtensor-stack/xtensor
.. _Rcpp: http://www.rcpp.org/
