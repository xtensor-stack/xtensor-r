# xtensor-r

**THIS IS A SUPER EARLY PREVIEW OF THE R BINDINGS, PLEASE DON'T USE IT JUST YET**

Bindings for xtensor to the R library.

## Test

Install `devtools` and `Rcpp` for R: 

```
install.packages('Rcpp')
install.packages('devtools')
```

And then, to build the `xtensor` R package, and install into the R workspace, run the following script. 
It will also call the `rcpp_hello_xtensor` function from `src/rcpp_hello_xtensor.cpp`.

```
cat install_and_test.R | R --no-save
```
