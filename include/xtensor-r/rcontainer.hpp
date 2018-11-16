/***************************************************************************
* Copyright (c) 2016, Wolf Vollprecht, Johan Mabille and Sylvain Corlay    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XTENSOR_R_CONTAINER_HPP
#define XTENSOR_R_CONTAINER_HPP

#include <complex>
#include <functional>
#include <numeric>

#include "xtensor/xbuffer_adaptor.hpp"
#include "xtensor/xcontainer.hpp"

#include "xtl/xsequence.hpp"

#include <Rcpp.h>
#include <RcppCommon.h>

namespace xt
{

    namespace detail
    {
        inline xbuffer_adaptor<int*> r_shape_to_buffer_adaptor(SEXP exp)
        {
            if (Rf_isNull(Rf_getAttrib(exp, R_DimSymbol)))
            {
                auto d = Rcpp::IntegerVector::create(Rf_length(exp));
                Rf_setAttrib(exp, R_DimSymbol, d);
            }

            SEXP shape_sexp = Rf_getAttrib(exp, R_DimSymbol);
            std::size_t n = (std::size_t)Rf_xlength(shape_sexp);
            return xbuffer_adaptor<int*>(
                Rcpp::internal::r_vector_start<INTSXP>(shape_sexp), n);
        }

        inline xbuffer_adaptor<int*> r_shape_to_buffer_adaptor(SEXP exp, std::size_t n)
        {
            if (Rf_isNull(Rf_getAttrib(exp, R_DimSymbol)))
            {
                auto d = Rcpp::IntegerVector::create(Rf_length(exp));
                Rf_setAttrib(exp, R_DimSymbol, d);
            }

            SEXP shape_sexp = Rf_getAttrib(exp, R_DimSymbol);
            if (n != (std::size_t)Rf_xlength(shape_sexp))
            {
                throw std::runtime_error("Could not convert shape. Dimensions don't match.");
            }
            return xbuffer_adaptor<int*>(
                Rcpp::internal::r_vector_start<INTSXP>(shape_sexp), n);
        }

        template <class T>
        inline const char* type_to_string(T) { return "unregistered type."; }
        inline const char* type_to_string(Rbyte) { return "Raw (Rbyte)"; }
        inline const char* type_to_string(double) { return "Numeric (double)"; }
        inline const char* type_to_string(int) { return "Integer (int32)"; }
        inline const char* type_to_string(bool) { return "Logical (bool)"; }
        inline const char* type_to_string(std::complex<double>) { return "Complex (std::complex<double>>)"; }

        inline const char* rtype_to_string(int rtype)
        {
            switch(rtype)
            {
                case REALSXP: return "Real";
                case INTSXP: return "Int";
                case LGLSXP: return "Logical";
                case STRSXP: return "String";
                case CPLXSXP: return "Complex";
                case RAWSXP: return "Raw";
                default: return "Unknown / Unmapped";
            }
        }
    }


    /**
     * @class rcontainer
     * @brief Base class for xtensor containers wrapping R arryays.
     *
     * The rcontainer class should not be instantiated directly. Instead, used should
     * use rtensor and rarray instances.
     *
     * @tparam D The derived type, i.e. the inheriting class for which rcontainer
     *           provides the interface.
     */
    template <class D>
    class rcontainer : public xcontainer<D>
    {
    public:

        using derived_type = D;

        using base_type = xcontainer<D>;
        using inner_types = xcontainer_inner_types<D>;
        using storage_type = typename inner_types::storage_type;
        using value_type = typename storage_type::value_type;
        using reference = typename storage_type::reference;
        using const_reference = typename storage_type::const_reference;
        using pointer = typename storage_type::pointer;
        using const_pointer = typename storage_type::const_pointer;
        using size_type = typename storage_type::size_type;
        using difference_type = typename storage_type::difference_type;

#ifndef XTENSOR_R_ALLOW_REINTERPRETATION
        static_assert(xtl::disjunction<std::is_same<value_type, int32_t>,
                                       std::is_same<value_type, double>,
                                       std::is_same<value_type, Rbyte>,
                                       // std::is_same<value_type, bool>, NOT YET IMPLEMENTED!
                                       std::is_same<value_type, std::complex<double>>>::value == true,
                      "R containers can only be of type bool, int, double, complex<double>.");
#endif
        using shape_type = typename inner_types::shape_type;
        using strides_type = typename inner_types::strides_type;
        using backstrides_type = typename inner_types::backstrides_type;
        using inner_shape_type = typename inner_types::inner_shape_type;
        using inner_strides_type = typename inner_types::inner_strides_type;

        using iterable_base = xiterable<D>;

        using iterator = typename iterable_base::iterator;
        using const_iterator = typename iterable_base::const_iterator;

        using stepper = typename iterable_base::stepper;
        using const_stepper = typename iterable_base::const_stepper;

        static constexpr layout_type static_layout = layout_type::column_major;
        static constexpr bool contiguous_layout = true;

        template <class S = shape_type>
        void resize(S&& shape);

        template <class S = shape_type>
        void reshape(S&& shape);

        layout_type layout() const;

        operator SEXP() const;
        void set_sexp(SEXP exp);

    protected:

        rcontainer();
        ~rcontainer();

        rcontainer(SEXP exp);

        rcontainer(const rcontainer&) = default;
        rcontainer& operator=(const rcontainer&) = default;

        rcontainer(rcontainer&&) = default;
        rcontainer& operator=(rcontainer&&) = default;

        // TODO: remove these once xcontainer::derived_cast is protected.
        derived_type& derived_cast() & noexcept;
        const derived_type& derived_cast() const & noexcept;
        derived_type derived_cast() && noexcept;

    private:

        SEXP m_sexp;
        bool m_owned;
    };


    template <class D>
    inline rcontainer<D>::rcontainer()
        : m_sexp(R_NilValue), m_owned(true)
    {
    }

    template <class D>
    inline rcontainer<D>::rcontainer(SEXP exp)
        : m_sexp(R_NilValue), m_owned(false)
    {
        set_sexp(exp);
        m_sexp = Rcpp::Rcpp_ReplaceObject(m_sexp, exp);
    }

    template <class D>
    inline rcontainer<D>::~rcontainer()
    {
        if (m_owned)
        {
            Rcpp::Rcpp_ReleaseObject(m_sexp);
            m_sexp = R_NilValue;
        }
    }

    template <class D>
    inline void rcontainer<D>::set_sexp(SEXP exp)
    {
        if (TYPEOF(exp) != D::SXP)
        {
            Rcpp::stop("R input has the wrong type. Expected %s, got %s",
                       detail::type_to_string(value_type{}), detail::rtype_to_string(TYPEOF(exp)));
        }

        m_sexp = Rcpp::Rcpp_ReplaceObject(m_sexp, exp);
    }

    /**
     * Resizes the container.
     * @param shape the new shape
     */
    template <class D>
    template <class S>
    inline void rcontainer<D>::resize(S&& shape)
    {
        if (shape.size() != this->dimension() || !std::equal(std::begin(shape), std::end(shape), this->shape().cbegin()))
        {
            derived_type tmp(xtl::forward_sequence<shape_type>(shape));
            *static_cast<derived_type*>(this) = std::move(tmp);
        }
    }

    /**
     * Reshapes the container.
     * @param shape the new shape
     */
    template <class D>
    template <class S>
    inline void rcontainer<D>::reshape(S&& shape)
    {
        if (compute_size(shape) != this->size())
        {
            throw std::runtime_error("Cannot reshape with incorrect number of elements.");
        }

        if (shape.size() != this->dimension() || !std::equal(std::begin(shape), std::end(shape), this->shape().cbegin()))
        {
            auto tmp_shape = Rcpp::IntegerVector(std::begin(shape), std::end(shape));
            Rf_setAttrib(m_sexp, R_DimSymbol, SEXP(tmp_shape));
            this->derived_cast().set_shape();
        }
    }

    template <class D>
    inline layout_type rcontainer<D>::layout() const
    {
        return layout_type::column_major;
    }

    template <class D>
    inline rcontainer<D>::operator SEXP() const
    {
        return m_sexp;
    }

    template <class D>
    inline auto rcontainer<D>::derived_cast() & noexcept -> derived_type&
    {
        return *static_cast<derived_type*>(this);
    }

    template <class D>
    inline auto rcontainer<D>::derived_cast() const & noexcept -> const derived_type&
    {
        return *static_cast<const derived_type*>(this);
    }

    template <class D>
    inline auto rcontainer<D>::derived_cast() && noexcept -> derived_type
    {
        return *static_cast<derived_type*>(this);
    }
}

#endif
