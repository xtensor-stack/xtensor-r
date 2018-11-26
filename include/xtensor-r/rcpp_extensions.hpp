/***************************************************************************
* Copyright (c) 2017, Wolf Vollprecht, Johan Mabille, and Sylvain Corlay   *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XTENSOR_R_RCPP_EXTENSIONS_HPP
#define XTENSOR_R_RCPP_EXTENSIONS_HPP

#include <Rcpp.h>

namespace xt
{
    template <class T>
    class rarray;

    template <class T, std::size_t N>
    class rtensor;
}

namespace Rcpp
{
    namespace traits
    {
        // Specializing these traits here prevents copies from the default
        // exporter in the generic `as<T>` function from Rcpp.
        template <class T>
        class Exporter<xt::rarray<T>>
        {
        public:

            Exporter(SEXP x)
                : m_sexp(x)
            {
            }

            inline xt::rarray<T> get()
            {
                return xt::rarray<T>(m_sexp);
            }

        private:

            SEXP m_sexp;
        };

        template <class T, std::size_t N>
        class Exporter<xt::rtensor<T, N>>
        { 
        public:

            Exporter(SEXP x)
                : m_sexp(x)
            {
            }

            inline xt::rtensor<T, N> get()
            {
                return xt::rtensor<T, N>(m_sexp);
            }

        private:

            SEXP m_sexp;
        };
    }
}

#endif
