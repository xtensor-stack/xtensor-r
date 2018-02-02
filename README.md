# ![xtensor-r](http://quantstack.net/assets/images/xtensor-r.svg)

[![Travis](https://travis-ci.org/QuantStack/xtensor-r.svg?branch=master)](https://travis-ci.org/QuantStack/xtensor-r)
[![Appveyor](https://ci.appveyor.com/api/projects/status/5pe90pdw4wddaxx7?svg=true)](https://ci.appveyor.com/project/QuantStack/xtensor-r)
[![Documentation](http://readthedocs.org/projects/xtensor-r/badge/?version=latest)](https://xtensor-r.readthedocs.io/en/latest/?badge=latest)
[![Join the Gitter Chat](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/QuantStack/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

R bindings for the [xtensor](https://github.com/QuantStack/xtensor) C++ multi-dimensional array library.

 - `xtensor` is a C++ library for multi-dimensional arrays enabling numpy-style broadcasting and lazy computing.
 - `xtensor-r` enables inplace use of R arrays in C++ with all the benefits from `xtensor`

     - C++ universal functions and broadcasting 
     - STL - compliant APIs.
     - A broad coverage of numpy APIs (see [the numpy to xtensor cheat sheet](http://xtensor.readthedocs.io/en/latest/numpy.html)).

`xtensor-r` can be used either to author C++ extensions for R with [Rcpp](https://github.com/RcppCore/Rcpp), or applications that embed the R interpreter with [RInside](https://github.com/eddelbuettel/rinside).

## Installation

`xtensor-r` has been packaged for CRAN (The Comprehensive R Archive Network). It can be installed

```R
install.packages("xtensor")
```

## Installation from Sources

`xtensor-r` is primarily a C++ library that can be installed classically using cmake in any installation prefix. For example, on unix systems

```bash
cmake -D CMAKE_INSTALL_PREFIX=/prefix/path/ .
make
make install
```

A tarball for the R package archive network (CRAN) can be generated.

```bash
mkdir build
cd build
cmake -D CMAKE_INSTALL_PREFIX=/prefix/path/ ..
make 
make cran
```

The generated tarball vendors the headers of the core `xtensor` library. It can be installed with

```R
install.packages('xtensor_0.2.0-0.tar.gz')
```

The package can be tested with a simple function call.

```R 
library('xtensor')
xtensor::rcpp_hello_xtensor(4:12)
```

## Dependencies on `xtensor` and `Rcpp`

`xtensor-r` depends on the `xtensor` and `Rcpp` libraries

| `xtensor-r`  | `xtensor` | `Rcpp`   |
|--------------|-----------|----------|
| master       |  ^0.15.1  | ^0.12.11 |
| 0.4.0        |  ^0.14.1  | ^0.12.11 |
| 0.3.0        |  ^0.13.1  | ^0.12.11 |
| 0.2.0        |  ^0.12..  | ^0.12.11 |
| 0.1.1        |  ^0.11.2  | ^0.12.11 |

 - Core `xtensor` headers are vendored in the CRAN package.
 - When using the conda, or debian package managers, xtensor-r has a dependency on xtensor.

## License

We use a shared copyright model that enables all contributors to maintain the copyright on their contributions.

This software is licensed under the BSD-3-Clause license. See the [LICENSE](LICENSE) file for details.
