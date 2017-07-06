# ![xtensor-r](http://quantstack.net/assets/images/xtensor-r.svg)

[![Travis](https://travis-ci.org/QuantStack/xtensor-r.svg?branch=master)](https://travis-ci.org/QuantStack/xtensor-r)
[![Appveyor](https://ci.appveyor.com/api/projects/status/5pe90pdw4wddaxx7?svg=true)](https://ci.appveyor.com/project/QuantStack/xtensor-r)
[![Join the Gitter Chat](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/QuantStack/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

**THIS IS A SUPER EARLY PREVIEW OF THE R BINDINGS, PLEASE DON'T USE IT JUST YET**

R bindings for the [xtensor](https://github.com/QuantStack/xtensor) C++ multi-dimensional array library.

 - `xtensor` is a C++ library for multi-dimensional arrays enabling numpy-style broadcasting and lazy computing.
 - `xtensor-r` enables inplace use of R arrays in C++ with all the benefits from `xtensor`

     - C++ universal function and broadcasting 
     - STL - compliant APIs.
     - A broad coverage of numpy APIs (see [the numpy to xtensor cheat sheet](http://xtensor.readthedocs.io/en/latest/numpy.html)).

The R bindings for `xtensor` are based on the [Rcpp](https://github.com/JuliaInterop/CxxWrap.jl/) C++ library.

## Installing from source

First, assemble the source tarbal for the CRAN package.

```bash
bash ./r-packaging.sh
```

From the `build` Install `devtools` and `Rcpp` for R: 

```R
# Rcpp
install.packages('Rcpp', repos='http://cran.us.r-project.org')
library('Rcpp')
Rcpp::compileAttributes('R-package')

# Devtools
install.packages('devtools', repos='http://cran.us.r-project.org')
library('devtools')
build('R-package')
```

At this stage, we have generated the `xtensor_0.1.0-0.tar.gz` file in the build directory which is the tarball that can be uploaded on CRAN.

It can be installed with

```R
install.packages('xtensor_0.1.0-0.tar.gz')
```

Testing the library

```R 
library('xtensor')
xtensor::rcpp_hello_xtensor(4:12)
```

## License

We use a shared copyright model that enables all contributors to maintain the copyright on their contributions.

This software is licensed under the BSD-3-Clause license. See the [LICENSE](LICENSE) file for details.
