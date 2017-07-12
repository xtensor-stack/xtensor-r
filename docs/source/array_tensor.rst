.. Copyright (c) 2016, Wolf Vollprecht, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Arrays and tensors
==================

``xtensor-r`` provides two container types wrapping R arrays: ``rarray`` and ``rtensor``. They are the counterparts
to ``xarray`` and ``xtensor`` containers.

rarray
------

Like ``xarray``, ``rarray`` has a dynamic shape. This means that you can reshape the R array on the C++ side and see this change reflected on the R side. ``rarray`` doesn't make a copy of the shape, but reads it each time it is needed. Therefore, if a reference on a ``rarray`` is kept in the C++ code and the corresponding R array is then reshaped in the R code, this modification will reflect in the ``rarray``.

rtensor
-------

Like ``xtensor``, ``rtensor`` has a static stack-allocated shape. This means that the shape of the R array is copied into the shape of the ``rtensor`` upon creation. As a consequence, reshapes are not reflected across languages. However, this drawback
is offset by a more effective computation of shape and broadcast.

