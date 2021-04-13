# ![xtensor-r](docs/source/xtensor-r.svg)

[![Travis](https://travis-ci.org/xtensor-stack/xtensor-r.svg?branch=master)](https://travis-ci.org/xtensor-stack/xtensor-r)
[![Appveyor](https://ci.appveyor.com/api/projects/status/xulke75tfifvnmj6?svg=true)](https://ci.appveyor.com/project/xtensor-stack/xtensor-r)
[![Documentation](http://readthedocs.org/projects/xtensor-r/badge/?version=latest)](https://xtensor-r.readthedocs.io/en/latest/?badge=latest)
[![Join the Gitter Chat](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/QuantStack/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

R bindings for the [xtensor](https://github.com/xtensor-stack/xtensor) C++ multi-dimensional array library.

 - `xtensor` is a C++ library for multi-dimensional arrays enabling numpy-style broadcasting and lazy computing.
 - `xtensor-r` enables inplace use of R arrays in C++ with all the benefits from `xtensor`

     - C++ universal functions and broadcasting
     - STL - compliant APIs.
     - A broad coverage of numpy APIs (see [the numpy to xtensor cheat sheet](http://xtensor.readthedocs.io/en/latest/numpy.html)).

`xtensor-r` can be used either to author C++ extensions for R with [Rcpp](https://github.com/RcppCore/Rcpp), or applications that embed the R interpreter with [RInside](https://github.com/eddelbuettel/rinside).

## Example

```cpp
#include <numeric>                    // Standard library import for std::accumulate
#define STRICT_R_HEADERS              // Otherwise a PI macro is defined in R
#include "xtensor/xmath.hpp"          // xtensor import for the C++ universal functions
#include "xtensor-r/rarray.hpp"       // R bindings

#include <Rcpp.h>

using namespace Rcpp;

// [[Rcpp::plugins(cpp14)]]

// [[Rcpp::export]]
double sum_of_sines(xt::rarray<double>& m)
{
    auto sines = xt::sin(m);  // sines does not actually hold values.
    return std::accumulate(sines.cbegin(), sines.cend(), 0.0);
}
```

```R
v <- matrix(0:14, nrow=3, ncol=5)
s <- sum_of_sines(v)
s

# prints 1.2853996391883833
```

## Installation

### Installation of the standalone C++ library

`xtensor-r` a header-only C++ library. It has been packaged for the mamba (or conda) package manager.

```bash
mamba install xtensor-r -c conda-forge
```

`xtensor-r` can be installed from source with cmake in any installation prefix. For example, on unix systems

```bash
cmake -D CMAKE_INSTALL_PREFIX=/prefix/path/ .
make
make install
```

### Installation of the R package

We provide a R package for Xtensor on both conda and CRAN (Comprehensive R Archive Network).
The packaging boilerplate for the R package is available at https://github.com/xtensor-stack/Xtensor.R.

To install the R package with conda:

```bash
conda install r-xtensor -c conda-forge
```

To install the R package from CRAN:

```bash
R CMD INSTALL xtensor
```

or from the GitHub repository using devtools

```R
devtools::install_github("xtensor-stack/Xtensor.R")
```

The CRAN package vendors the headers for `xtensor-r`, xtensor`, `xtl` and `xsimd`.

## Documentation

To get started with using `xtensor-r`, check out the full documentation

http://xtensor-r.readthedocs.io/

## Dependencies on `xtensor` and `Rcpp`

`xtensor-r` depends on the `xtensor` and `Rcpp` libraries

| `xtensor-r`  | `xtensor` | `Rcpp`   |
|--------------|-----------|----------|
| master       |  ^0.23.0  | ^1.0     |
| 0.13.0       |  ^0.23.0  | ^1.0     |
| 0.12.1       |  ^0.21.4  | ^1.0     |
| 0.12.0       |  ^0.21.2  | ^1.0     |
| 0.11.1       |  ^0.20.8  | ^1.0     |
| 0.11.0       |  ^0.20.4  | ^1.0     |
| 0.10.0       |  ^0.19.4  | ^1.0     |
| 0.9.0        |  ^0.19.0  | ^1.0     |
| 0.8.2        |  ^0.18.2  | ^0.12.11 |
| 0.8.1        |  ^0.18.1  | ^0.12.11 |
| 0.8.0        |  ^0.18.0  | ^0.12.11 |

 - Core `xtensor` headers are vendored in the CRAN package.
 - When using the conda, or debian package managers, xtensor-r has a dependency on xtensor.

## License

We use a shared copyright model that enables all contributors to maintain the copyright on their contributions.

This software is licensed under the BSD-3-Clause license. See the [LICENSE](LICENSE) file for details.
