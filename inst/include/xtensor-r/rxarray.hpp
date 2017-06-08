/***************************************************************************
* Copyright (c) 2017, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef RXARRAY_HPP
#define RXARRAY_HPP

#include <xtensor/xcontainer.hpp>
#include <xtensor/xbuffer_adaptor.hpp>
#include <xtensor/xutils.hpp>
#include <xtensor/xsemantic.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xiterator.hpp>

#include <RcppCommon.h>
#include <Rcpp.h>

using namespace Rcpp;
using namespace xt;

namespace xt
{

    template <class T>
    class rxarray;

    template <class T>
    struct xcontainer_inner_types<rxarray<T>>
    {
        using container_type = xbuffer_adaptor<T>;
        using shape_type = std::vector<typename container_type::size_type>;
        using strides_type = shape_type;
        using backstrides_type = shape_type;
        using inner_shape_type = IntegerVector;
        using inner_strides_type = shape_type;
        using inner_backstrides_type = backstrides_type;
        using temporary_type = rxarray<T>;
    };

    template <class T>
    struct xiterable_inner_types<rxarray<T>>
        : xcontainer_iterable_types<rxarray<T>>
    {
    };

    template<class T>
    class rxarray : public xcontainer<rxarray<T>>,
                    public xcontainer_semantic<rxarray<T>>
    {

    public:
        
        using self_type = rxarray<T>;
        using base_type = xcontainer<self_type>;
        using semantic_base = xcontainer_semantic<rxarray<T>>;

        using inner_types = xcontainer_inner_types<self_type>;

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
        using inner_backstrides_type = typename inner_types::inner_backstrides_type;

        using iterable_base = xiterable<self_type>;

        using iterator = typename iterable_base::iterator;
        using const_iterator = typename iterable_base::const_iterator;

        using stepper = typename iterable_base::stepper;
        using const_stepper = typename iterable_base::const_stepper;

        static constexpr layout_type static_layout = layout_type::column_major;
        static constexpr bool contiguous_layout = true;

        constexpr static int SXP = traits::r_sexptype_traits<T>::rtype;

        rxarray(SEXP exp, bool owned = false);

        rxarray(const shape_type& shape, bool owned = true);
        rxarray(const shape_type& shape, const_reference value, bool owned = true);

        template <class E>
        rxarray(const xexpression<E>& e);

        rxarray(const self_type& rhs);

        rxarray(self_type&&) = default;
        self_type& operator=(self_type&&) = default;

        ~rxarray();

        template <class E>
        inline self_type& operator=(const xexpression<E>& e);

        template <class S = shape_type>
        inline void reshape(const S& shape);

        using base_type::begin;
        using base_type::end;

        inline layout_type layout() const;
        inline SEXP get_sexp() const;

    private:

        SEXP m_sexp;
        container_type m_data;
        inner_shape_type m_shape;
        strides_type m_strides;
        strides_type m_backstrides;
        bool m_owned;

        inline const inner_shape_type& shape_impl() const noexcept;
        inline const inner_strides_type& strides_impl() const noexcept;
        inline const inner_backstrides_type& backstrides_impl() const noexcept;
        inline container_type& data_impl() noexcept;
        inline const container_type& data_impl() const noexcept;

        friend class xcontainer<rxarray<T>>;
    };

    template <class T>
    rxarray<T>::rxarray(SEXP exp, bool owned)
        : m_sexp(exp), m_owned(owned)
    {
        m_shape = inner_shape_type(Rf_getAttrib(m_sexp, R_DimSymbol));

        resize_container(m_strides, base_type::dimension());
        resize_container(m_backstrides, base_type::dimension());

        xt::compute_strides(m_shape, layout(), m_strides, m_backstrides);

        std::size_t sz = compute_size(m_shape);
        m_data = container_type(internal::r_vector_start<SXP>(exp), sz);
    }

    template <class T>
    rxarray<T>::rxarray(const rxarray<T>::shape_type& shape, bool owned)
        : m_owned(owned)
    {
        resize_container(m_strides, shape.size());
        resize_container(m_backstrides, shape.size());

        auto tmp_shape = IntegerVector(shape.begin(), shape.end());

        xt::compute_strides(shape, layout(), m_strides, m_backstrides);

        std::size_t sz = compute_size(shape);

        m_sexp = Rf_allocArray(SXP, SEXP(tmp_shape));
        m_data = container_type(internal::r_vector_start<SXP>(m_sexp), sz);

        m_shape = inner_shape_type(Rf_getAttrib(m_sexp, R_DimSymbol));

        if (m_owned)
        {
            Rcpp_PreserveObject(m_sexp);
        }
    }

    template <class T>
    rxarray<T>::rxarray(const rxarray<T>::shape_type& shape, const_reference value, bool owned)
        : rxarray<T>(shape, owned)
    {
        std::fill(begin(), end(), value);
    }

    template <class T>
    template <class E>
    rxarray<T>::rxarray(const xexpression<E>& e)
    {
        shape_type shape = forward_sequence<shape_type>(e.derived_cast().shape());
        semantic_base::assign(e);
    }

    template <class T>
    rxarray<T>::rxarray(const self_type& rhs)
    {
        m_strides = rhs.strides();
        m_backstrides = rhs.backstrides();

        std::size_t sz = compute_size(m_shape);
        m_sexp = Rf_allocArray(SXP, SEXP(rhs.shape()));
        m_data = container_type(internal::r_vector_start<SXP>(m_sexp), sz);
        m_shape = inner_shape_type(Rf_getAttrib(m_sexp, R_DimSymbol));

        std::copy(rhs.data().cbegin(), rhs.data().cend(), this->data().begin());
    }

    template <class T>
    rxarray<T>::~rxarray<T>()
    {
        if (m_owned)
        {
            Rcpp_ReleaseObject(m_sexp);
        }
    }

    template <class T>
    template <class E>
    inline auto rxarray<T>::operator=(const xexpression<E>& e) -> self_type&
    {
        return semantic_base::operator=(e);
    }

    template <class T>
    template <class S>
    inline void rxarray<T>::reshape(const S& shape)
    {
        self_type tmp(shape);
        *static_cast<self_type*>(this) = std::move(tmp);
    }        

    template <class T>
    inline layout_type rxarray<T>::layout() const
    {
        return layout_type::column_major;
    }

    template <class T>
    inline SEXP rxarray<T>::get_sexp() const
    {
        return m_sexp;
    }

    template <class T>
    inline auto rxarray<T>::shape_impl() const noexcept -> const inner_shape_type&
    {
        return m_shape;
    }

    template <class T>
    inline auto rxarray<T>::strides_impl() const noexcept -> const inner_strides_type&
    {
        return m_strides;
    }

    template <class T>
    inline auto rxarray<T>::backstrides_impl() const noexcept -> const inner_backstrides_type&
    {
        return m_backstrides;
    }

    template <class T>
    inline auto rxarray<T>::data_impl() noexcept -> container_type&
    {
        return m_data;
    }

    template <class T>
    inline auto rxarray<T>::data_impl() const noexcept -> const container_type&
    {
        return m_data;
    }
}

namespace Rcpp
{
    template <typename T>
    SEXP wrap(const xt::rxarray<T>& arr)
    {
        return arr.get_sexp();
    }
}

#endif