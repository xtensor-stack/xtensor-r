/***************************************************************************
* Copyright (c) 2016, Wolf Vollprecht, Johan Mabille and Sylvain Corlay    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef R_CONTAINER_HPP
#define R_CONTAINER_HPP

#include <functional>
#include <numeric>

#include "xtensor/xbuffer_adaptor.hpp"
#include "xtensor/xcontainer.hpp"

#include "xtl/xsequence.hpp"

#include <Rcpp.h>
#include <RcppCommon.h>

namespace xt
{

    using Rcpp::Rcpp_ReplaceObject;
    using Rcpp::Rcpp_ReleaseObject;

    namespace detail
    {
        inline xbuffer_adaptor<int> r_shape_to_buffer_adaptor(SEXP exp)
        {
            if (Rf_isNull(Rf_getAttrib(exp, R_DimSymbol)))
            {
                Rcpp::IntegerVector d = Rcpp::IntegerVector::create(Rf_length(exp));
                Rf_setAttrib(exp, R_DimSymbol, d);
            }

            SEXP shape_sexp = Rf_getAttrib(exp, R_DimSymbol);
            std::size_t n = (std::size_t)Rf_xlength(shape_sexp);
            return xbuffer_adaptor<int>(
                Rcpp::internal::r_vector_start<INTSXP>(shape_sexp), n);
        }

        inline xbuffer_adaptor<int> r_shape_to_buffer_adaptor(SEXP exp, std::size_t n)
        {
            if (Rf_isNull(Rf_getAttrib(exp, R_DimSymbol)))
            {
                Rcpp::IntegerVector d = Rcpp::IntegerVector::create(Rf_length(exp));
                Rf_setAttrib(exp, R_DimSymbol, d);
            }

            SEXP shape_sexp = Rf_getAttrib(exp, R_DimSymbol);
            if (n != (std::size_t)Rf_xlength(shape_sexp))
            {
                throw std::runtime_error("Could not convert shape. Dimensions don't match.");
            }
            return xbuffer_adaptor<int>(
                Rcpp::internal::r_vector_start<INTSXP>(shape_sexp), n);
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
        using container_type = typename inner_types::container_type;
        using value_type = typename container_type::value_type;
        using reference = typename container_type::reference;
        using const_reference = typename container_type::const_reference;
        using pointer = typename container_type::pointer;
        using const_pointer = typename container_type::const_pointer;
        using size_type = typename container_type::size_type;
        using difference_type = typename container_type::difference_type;

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

        void reshape(const shape_type& shape);
        void reshape(const shape_type& shape, const strides_type& strides);

        layout_type layout() const;

        operator SEXP() const;

        using base_type::operator();
        using base_type::operator[];
        using base_type::begin;
        using base_type::end;

        void set_sexp(SEXP exp);

    protected:

        rcontainer();
        ~rcontainer();

        rcontainer(SEXP exp);

        rcontainer(const rcontainer&) = default;
        rcontainer& operator=(const rcontainer&) = default;

        rcontainer(rcontainer&&) = default;
        rcontainer& operator=(rcontainer&&) = default;

    private:

        SEXP m_sexp;
        bool m_owned;
    };


    template <class D>
    rcontainer<D>::rcontainer()
        : m_sexp(R_NilValue), m_owned(true)
    {
    }

    template <class D>
    rcontainer<D>::rcontainer(SEXP exp)
        : m_sexp(R_NilValue), m_owned(false)
    {
        m_sexp = Rcpp_ReplaceObject(m_sexp, exp);
    }

    template <class D>
    rcontainer<D>::~rcontainer()
    {
        if (m_owned)
        {
            Rcpp_ReleaseObject(m_sexp);
            m_sexp = R_NilValue;
        }
    }

    template <class D>
    void rcontainer<D>::set_sexp(SEXP exp)
    {
        m_sexp = Rcpp_ReplaceObject(m_sexp, exp);
    }

    /**
     * Reshapes the container.
     * @param shape the new shape
     */
    template <class D>
    inline void rcontainer<D>::reshape(const shape_type& shape)
    {
        if (shape.size() != this->dimension() || !std::equal(shape.begin(), shape.end(), this->shape().begin()))
        {
            strides_type strides = xtl::make_sequence<strides_type>(shape.size(), size_type(1));
            compute_strides(shape, layout_type::column_major, strides);
            reshape(shape, strides);
        }
    }

    /**
     * Reshapes the container.
     * @param shape the new shape
     * @param strides the new strides
     */
    template <class D>
    inline void rcontainer<D>::reshape(const shape_type& shape, const strides_type& strides)
    {
        derived_type tmp(shape);
        *static_cast<derived_type*>(this) = std::move(tmp);
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
}

#endif
