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

Although ``xtensor-r`` is a header-only library, we provide standardized means to install it, with package managers or with cmake.

Besides the xtendor-r headers, all these methods place the `cmake` project configuration file in the right location so that third-party projects can use cmake's find_package to locate xtensor-r headers.

.. image:: cran.svg

Using the CRAN package
----------------------

A package for xtensor-r is available on CRAN (The Comprehensive R Archive Network).

.. code::

    install.packages("xtensor")

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

A tarball for the R package archive network (CRAN) can be generated.

.. code::

    mkdir build
    cd build
    cmake -D CMAKE_INSTALL_PREFIX=/prefix/path/ ..
    make 
    make cran

The generated tarball vendors the headers of the core xtensor library. It can be installed with

.. code::

    install.packages('xtensor_0.1.0-0.tar.gz')

