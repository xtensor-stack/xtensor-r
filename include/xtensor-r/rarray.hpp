/***************************************************************************
* Copyright (c) 2017, Wolf Vollprecht, Johan Mabille, and Sylvain Corlay   *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef RARRAY_HPP
#define RARRAY_HPP

#include "xtensor/xbuffer_adaptor.hpp"
#include "xtensor/xcontainer.hpp"
#include "xtensor/xiterator.hpp"
#include "xtensor/xsemantic.hpp"
#include "xtensor/xutils.hpp"

#include "xtensor-r/rcontainer.hpp"

#include <Rcpp.h>
#include <RcppCommon.h>

using namespace Rcpp;
using namespace xt;

namespace xt
{

    template <class T>
    class rarray;

    template <class T>
    struct xcontainer_inner_types<rarray<T>>
    {
        using container_type = xbuffer_adaptor<T>;
        using shape_type = std::vector<typename container_type::size_type>;
        using strides_type = shape_type;
        using backstrides_type = shape_type;
        using inner_shape_type = xbuffer_adaptor<int>;
        using inner_strides_type = shape_type;
        using inner_backstrides_type = backstrides_type;
        using temporary_type = rarray<T>;
        static constexpr layout_type layout = layout_type::column_major;
    };

    template <class T>
    struct xiterable_inner_types<rarray<T>>
        : xcontainer_iterable_types<rarray<T>>
    {
    };

    template <class T>
    class rarray : public rcontainer<rarray<T>>,
                   public xcontainer_semantic<rarray<T>>
    {
    public:

        using self_type = rarray<T>;
        using base_type = rcontainer<self_type>;
        using semantic_base = xcontainer_semantic<rarray<T>>;

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

        constexpr static int SXP = traits::r_sexptype_traits<T>::rtype;

        rarray();
        rarray(SEXP exp);

        rarray(const shape_type& shape);
        rarray(const shape_type& shape, const_reference value);

        rarray(const value_type& t);
        rarray(nested_initializer_list_t<value_type, 1> t);
        rarray(nested_initializer_list_t<value_type, 2> t);
        rarray(nested_initializer_list_t<value_type, 3> t);
        rarray(nested_initializer_list_t<value_type, 4> t);
        rarray(nested_initializer_list_t<value_type, 5> t);

        template <class E>
        rarray(const xexpression<E>& e);

        rarray(const self_type& rhs);
        self_type& operator=(const self_type& rhs);

        rarray(self_type&&) = default;
        self_type& operator=(self_type&&) = default;

        template <class E>
        self_type& operator=(const xexpression<E>& e);

        template <class S = shape_type>
        void reshape(const S& shape);

        using base_type::begin;
        using base_type::end;

        layout_type layout() const;

    private:

        container_type m_data;
        inner_shape_type m_shape;
        strides_type m_strides;
        strides_type m_backstrides;

        template <class S>
        void init_from_shape(const S& shape);

        const inner_shape_type& shape_impl() const noexcept;
        const inner_strides_type& strides_impl() const noexcept;
        const inner_backstrides_type& backstrides_impl() const noexcept;
        container_type& data_impl() noexcept;
        const container_type& data_impl() const noexcept;

        friend class xcontainer<rarray<T>>;
    };

    template <class T>
    inline rarray<T>::rarray()
        : base_type()
    {
    }

    template <class T>
    inline rarray<T>::rarray(SEXP exp)
        : base_type(exp)
    {
        m_shape = detail::r_shape_to_buffer_adaptor(*this);

        resize_container(m_strides, base_type::dimension());
        resize_container(m_backstrides, base_type::dimension());

        xt::compute_strides(m_shape, layout(), m_strides, m_backstrides);

        std::size_t sz = compute_size(m_shape);
        m_data = container_type(static_cast<T*>(internal::r_vector_start<SXP>(exp)), sz);
    }

    template <class T>
    template <class S>
    inline void rarray<T>::init_from_shape(const S& shape)
    {
        resize_container(m_strides, shape.size());
        resize_container(m_backstrides, shape.size());
        auto tmp_shape = IntegerVector(shape.begin(), shape.end());

        xt::compute_strides(shape, layout(), m_strides, m_backstrides);

        std::size_t sz = compute_size(shape);

        base_type::set_sexp(Rf_allocArray(SXP, SEXP(tmp_shape)));
        m_data = container_type(reinterpret_cast<T*>(internal::r_vector_start<SXP>(SEXP(*this))), sz);
        m_shape = detail::r_shape_to_buffer_adaptor(*this);
    }

    template <class T>
    inline rarray<T>::rarray(const shape_type& shape)
    {
        init_from_shape(shape);
    }

    template <class T>
    inline rarray<T>::rarray(const shape_type& shape, const_reference value)
    {
        init_from_shape(shape);
        std::fill(this->begin(), this->end(), value);
    }

    template <class T>
    template <class E>
    inline rarray<T>::rarray(const xexpression<E>& e)
    {
        semantic_base::assign(e);
    }

    template <class T>
    inline rarray<T>::rarray(const self_type& rhs)
    {
        init_from_shape(rhs.shape());
        std::copy(rhs.data().cbegin(), rhs.data().cend(), this->data().begin());
    }

    template <class T>
    inline rarray<T>::rarray(const value_type& t)
    {
        init_from_shape(xt::shape<shape_type>(t));
        nested_copy(m_data.begin(), t);
    }

    template <class T>
    inline rarray<T>::rarray(nested_initializer_list_t<value_type, 1> t)
    {
        init_from_shape(xt::shape<shape_type>(t));
        nested_copy(this->begin(), t);
    }

    template <class T>
    inline rarray<T>::rarray(nested_initializer_list_t<value_type, 2> t)
    {
        init_from_shape(xt::shape<shape_type>(t));
        nested_copy(this->begin(), t);
    }

    template <class T>
    inline rarray<T>::rarray(nested_initializer_list_t<value_type, 3> t)
    {
        init_from_shape(xt::shape<shape_type>(t));
        nested_copy(this->begin(), t);
    }

    template <class T>
    inline rarray<T>::rarray(nested_initializer_list_t<value_type, 4> t)
    {
        init_from_shape(xt::shape<shape_type>(t));
        nested_copy(this->begin(), t);
    }

    template <class T>
    inline rarray<T>::rarray(nested_initializer_list_t<value_type, 5> t)
    {
        init_from_shape(xt::shape<shape_type>(t));
        nested_copy(this->begin(), t);
    }

    template <class T>
    inline auto rarray<T>::operator=(const self_type& rhs) -> self_type&
    {
        self_type tmp(rhs);
        *this = std::move(tmp);
        return *this;
    }

    template <class T>
    template <class E>
    inline auto rarray<T>::operator=(const xexpression<E>& e) -> self_type&
    {
        return semantic_base::operator=(e);
    }

    template <class T>
    template <class S>
    inline void rarray<T>::reshape(const S& shape)
    {
        self_type tmp(shape);
        *static_cast<self_type*>(this) = std::move(tmp);
    }

    template <class T>
    inline layout_type rarray<T>::layout() const
    {
        return layout_type::column_major;
    }

    template <class T>
    inline auto rarray<T>::shape_impl() const noexcept -> const inner_shape_type&
    {
        return m_shape;
    }

    template <class T>
    inline auto rarray<T>::strides_impl() const noexcept -> const inner_strides_type&
    {
        return m_strides;
    }

    template <class T>
    inline auto rarray<T>::backstrides_impl() const noexcept -> const inner_backstrides_type&
    {
        return m_backstrides;
    }

    template <class T>
    inline auto rarray<T>::data_impl() noexcept -> container_type&
    {
        return m_data;
    }

    template <class T>
    inline auto rarray<T>::data_impl() const noexcept -> const container_type&
    {
        return m_data;
    }
}

namespace Rcpp
{
    template <typename T>
    SEXP wrap(const xt::rarray<T>& arr)
    {
        return SEXP(arr);
    }
}

#endif
