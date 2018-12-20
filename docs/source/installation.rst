.. Copyright (c) 2016, Wolf Vollprecht, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.


.. raw:: html

   <style>
   .rst-content .section>img {
       width: 30px;
       margin-bottom: 0;
       margin-top: 0;
       margin-right: 15px;
       margin-left: 15px;
       float: left;
   }
   </style>

Installation
============

``xtensor-r`` is a header-only C++ library. We maintain the conda package for xtensor-r and its dependencies.

Besides the xtendor-r headers, all these methods place the `cmake` project configuration file in the right location so that third-party projects can use cmake's find_package to locate xtensor-r headers.

.. image:: conda.svg

Using the conda package
-----------------------

A package for xtensor-r is available on the conda package manager.

.. code::

    conda install -c conda-forge xtensor-r

.. image:: cmake.svg

From source with cmake
----------------------

You can also install ``xtensor-r`` from source with cmake. On Unix platforms, from the source directory:

.. code::

    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=/path/to/prefix ..
    make install

On Windows platforms, from the source directory:

.. code::

    mkdir build
    cd build
    cmake -G "NMake Makefiles" -DCMAKE_INSTALL_PREFIX=/path/to/prefix ..
    nmake
    nmake install

.. image:: cran.svg

Using the R package
-------------------

We also provide a R package for xtensor, which has been packaged for both conda and CRAN (Comprehensive R Archive Network). The repository for the R package is https://github.com/QuantStack/Xtensor.R.

To install the conda package:

.. code::

    conda install r-xtensor -c conda-forge

To install the R package from CRAN

.. code::

    install.packages("xtensor")


