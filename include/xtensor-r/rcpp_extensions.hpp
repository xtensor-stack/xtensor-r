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

    template <class T>
    class rarray_optional;
    
    template <class T, std::size_t N>
    class rtensor_optional;
}

namespace Rcpp
{
    // Specializing Rcpp traits for xtensor types prevents copies
    // occuring in the generic `as<T>` function from Rcpp, called
    // from the generic exporter.
    namespace traits
    {
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

        template <class T>
        class Exporter<xt::rarray_optional<T>>
        {
        public:

            Exporter(SEXP x)
                : m_sexp(x)
            {
            }

            inline xt::rarray_optional<T> get()
            {
                return xt::rarray_optional<T>(m_sexp);
            }

        private:

            SEXP m_sexp;
        };
        
        template <class T, std::size_t N>
        class Exporter<xt::rtensor_optional<T, N>>
        {
        public:
            
            Exporter(SEXP x)
                : m_sexp(x)
            {
            }
            
            inline xt::rtensor_optional<T, N> get()
            {
                return xt::rtensor_optional<T, N>(m_sexp);
            }
            
        private:
            
            SEXP m_sexp;
        };
    }
}

#endif
