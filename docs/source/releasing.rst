.. Copyright (c) 2016, Wolf Vollprecht, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Releasing xtensor-r
===================

Releasing a new version
-----------------------

From the master branch of xtensor-r

- Make sure that you are in sync with the master branch of the upstream remote.
- In file ``xtensor_r_config.hpp``, set the macros for ``XTENSOR_R_VERSION_MAJOR``, ``XTENSOR_R_VERSION_MINOR`` and ``XTENSOR_R_VERSION_PATCH`` to the desired values.
- Update the readme file w.r.t. dependencies on xtensor and pybind11.
- Stage the changes (``git add``), commit the changes (``git commit``) and add a tag of the form ``Major.minor.patch``. It is important to not add any other content to the tag name.
- Push the new commit and tag to the main repository. (``git push``, and ``git push --tags``)

Updating the conda-forge recipe
-------------------------------

xtensor-r has been packaged for the conda package manager. Once the new tag has been pushed on GitHub, edit the conda-forge recipe for xtensor in the following fashion:

- Update the version number to the new Major.minor.patch.
- Set the build number to 0.
- Update the hash of the source tarball.
- Check for the versions of the dependencies.
- Optionally, rerender the conda-forge feedstock.

Releasing the R Package
=======================

To build a package tarball for R, simply clone https://github.com/xtensor-stack/Xtensor.R and run

.. code:: bash

    R CMD build .

from the root of the source directory.

To produce a package tarball *vendoring* the xtl, xsimd and xtensor dependencies (for e.g. CRAN), clone https://github.com/xtensor-stack/Xtensor.R and run

.. code:: bash

    ./configure   # Downloads xtl, xtensor, xsimd, and xtensor-r.
    R CMD build .

from the root of the source directory.
