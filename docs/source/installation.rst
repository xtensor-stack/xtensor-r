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

Installation of the xtensor-r C++ library
=========================================

``xtensor-r`` is a header-only C++ library. We maintain the conda package for xtensor-r and its dependencies.

Besides the xtendor-r headers, all these methods place the `cmake` project configuration file in the right location so that third-party projects can use cmake's find_package to locate xtensor-r headers.

.. image:: conda.svg

Using the conda-forge package
-----------------------------

A package for xtensor-r is available on the mamba (or conda) package manager.

.. code::

    mamba install -c conda-forge xtensor-r

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

Installation of the R package
=============================

We provide a R package for Xtensor on both conda and CRAN (Comprehensive R Archive Network).
The packaging boilerplate for the R package is available at https://github.com/xtensor-stack/Xtensor.R.

To install the R package with conda:

.. code:: bash

    conda install r-xtensor -c conda-forge

To install the R package from CRAN:

.. code:: bash

    R CMD INSTALL xtensor

or from the GitHub repository using devtools:

.. code:: r

    devtools::install_github("xtensor-stack/Xtensor.R")

.. note::

   A key difference between the version of the R package available on CRAN and conda is that

    - the CRAN package *vendors* the headers of ``xtl``, ``xsimd``, ``xtensor``, and ``xtensor-r``.
    - the conda package *depends* on the conda packages for ``xtl``, ``xsimd``, ``xtensor`` and ``xtensor-r``.

   When installing the package from CRAN, it is still possible to drop the vendored dependencies by adding the
   ``--configure-args='--novendor'`` option to the ``R CMD INSTALL`` command, or by defining the ``NO_VENDOR_XTENSOR``
   environment variable to ``YES``.

