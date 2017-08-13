/***************************************************************************
* Copyright (c) 2017, Wolf Vollprecht, Johan Mabille and Sylvain Corlay    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef R_TENSOR_HPP
#define R_TENSOR_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>

#include "xtensor/xbuffer_adaptor.hpp"
#include "xtensor/xio.hpp"
#include "xtensor/xiterator.hpp"
#include "xtensor/xsemantic.hpp"
#include "xtensor/xutils.hpp"

#include <Rcpp.h>
#include <RcppCommon.h>

using namespace Rcpp;

#include "rcontainer.hpp"

namespace xt
{

    template <class T, std::size_t N>
    class rtensor;

    template <class T, std::size_t N>
    struct xiterable_inner_types<rtensor<T, N>>
        : xcontainer_iterable_types<rtensor<T, N>>
    {
    };

    template <class T, std::size_t N>
    struct xcontainer_inner_types<rtensor<T, N>>
    {
        using container_type = xbuffer_adaptor<T>;
        using shape_type = std::array<int, N>;
        using strides_type = shape_type;
        using backstrides_type = shape_type;
        using inner_shape_type = xbuffer_adaptor<int>;
        using inner_strides_type = strides_type;
        using inner_backstrides_type = backstrides_type;
        using temporary_type = rtensor<T, N>;
    };

    /**
     * @class rtensor
     * @brief Multidimensional container providing the xtensor container semantics wrapping a R array.
     *
     * rtensor is similar to the xtensor container in that it has a static dimensionality.
     *
     * Unlike with the pyarray container, rtensor cannot be reshaped with a different number of dimensions
     * and reshapes are not reflected on the Python side. However, rtensor has benefits compared to pyarray
     * in terms of performances. rtensor shapes are stack-allocated which makes iteration upon rtensor
     * faster than with pyarray.
     *
     * @tparam T The type of the element stored in the pyarray.
     * @sa pyarray
     */
    template <class T, std::size_t N>
    class rtensor : public rcontainer<rtensor<T, N>>,
                    public xcontainer_semantic<rtensor<T, N>>
    {
    public:

        using self_type = rtensor<T, N>;
        using semantic_base = xcontainer_semantic<self_type>;
        using base_type = rcontainer<self_type>;
        using container_type = typename base_type::container_type;
        using value_type = typename base_type::value_type;
        using reference = typename base_type::reference;
        using const_reference = typename base_type::const_reference;
        using pointer = typename base_type::pointer;
        using size_type = typename base_type::size_type;
        using shape_type = typename base_type::shape_type;
        using strides_type = typename base_type::strides_type;
        using backstrides_type = typename base_type::backstrides_type;
        using inner_shape_type = typename base_type::inner_shape_type;
        using inner_strides_type = typename base_type::inner_strides_type;
        using inner_backstrides_type = typename base_type::inner_backstrides_type;

        constexpr static int SXP = traits::r_sexptype_traits<T>::rtype;

        rtensor();
        rtensor(nested_initializer_list_t<T, N> t);

        explicit rtensor(SEXP exp);
        explicit rtensor(const shape_type& shape);
        explicit rtensor(const shape_type& shape, const_reference value);

        template <class S>
        static self_type from_shape(const S& shape);

        rtensor(const self_type& rhs);
        self_type& operator=(const self_type& rhs);

        rtensor(self_type&&) = default;
        self_type& operator=(self_type&& e) = default;

        template <class E>
        rtensor(const xexpression<E>& e);

        template <class E>
        self_type& operator=(const xexpression<E>& e);

        template <class S>
        inline void reshape(const S& shape);

        layout_type layout() const;

        using base_type::begin;
        using base_type::end;
        using base_type::xbegin;
        using base_type::xend;

    private:

        inner_shape_type m_shape;
        inner_strides_type m_strides;
        inner_backstrides_type m_backstrides;
        container_type m_data;

        template <class S>
        void init_from_shape(const S& shape);

        inner_shape_type& shape_impl() noexcept;
        const inner_shape_type& shape_impl() const noexcept;
        inner_strides_type& strides_impl() noexcept;
        const inner_strides_type& strides_impl() const noexcept;
        inner_backstrides_type& backstrides_impl() noexcept;
        const inner_backstrides_type& backstrides_impl() const noexcept;

        container_type& data_impl() noexcept;
        const container_type& data_impl() const noexcept;

        friend class xcontainer<rtensor<T, N>>;
    };

    /***************************
     * rtensor implementation *
     ***************************/

    /**
     * @name Constructors
     */
    //@{
    /**
     * Allocates an uninitialized rtensor that holds 1 element.
     */
    template <class T, std::size_t N>
    inline rtensor<T, N>::rtensor()
        : base_type()
    {
        auto tmp_shape = IntegerVector(N, 1);
        xt::compute_strides(tmp_shape, layout_type::column_major, m_strides, m_backstrides);
        // Workaround. Rcpp's IntegerVector lacks cbegin() and cend() methods
        // which are used in compute_size().
        auto adaptor = xbuffer_adaptor<int>(&(tmp_shape[0]), N);
        std::size_t sz = compute_size(adaptor);

        base_type::set_sexp(Rf_allocArray(SXP, SEXP(tmp_shape)));
        m_data = container_type(internal::r_vector_start<SXP>(SEXP(*this)), sz);
        m_shape = detail::r_shape_to_buffer_adaptor(*this, N);

        m_data[0] = T();
    }

    template <class T, std::size_t N>
    inline rtensor<T, N>::rtensor(SEXP exp)
        : base_type(exp)
    {
        m_shape = detail::r_shape_to_buffer_adaptor(*this, N);
        xt::compute_strides(m_shape, layout_type::column_major, m_strides, m_backstrides);
        std::size_t sz = compute_size(m_shape);
        m_data = container_type(internal::r_vector_start<SXP>(SEXP(*this)), sz);
    }


    template <class T, std::size_t N>
    template <class S>
    void rtensor<T, N>::init_from_shape(const S& shape)
    {
        auto tmp_shape = IntegerVector(shape.begin(), shape.end());
        xt::compute_strides(shape, layout_type::column_major, m_strides, m_backstrides);

        std::size_t sz = compute_size(shape);

        base_type::set_sexp(Rf_allocArray(SXP, SEXP(tmp_shape)));
        m_data = container_type(internal::r_vector_start<SXP>(SEXP(*this)), sz);

        m_shape = detail::r_shape_to_buffer_adaptor(*this, N);
    }

    template <class T, std::size_t N>
    template <class S>
    inline rtensor<T, N> rtensor<T, N>::from_shape(const S& shape)
    {
        shape_type temp_shape = forward_sequence<shape_type>(shape);
        return self_type(temp_shape);
    }

    /**
     * Allocates a rtensor with a nested initializer list.
     */
    template <class T, std::size_t N>
    inline rtensor<T, N>::rtensor(nested_initializer_list_t<T, N> t)
        : self_type(xt::shape<shape_type>(t))
    {
        // always using xbegin as it's column_major layout
        nested_copy(xbegin(), t);
    }

    /**
     * Allocates an uninitialized rtensor with the specified shape and
     * layout.
     * @param shape the shape of the rtensor
     * @param l the layout_type of the rtensor
     */
    template <class T, std::size_t N>
    inline rtensor<T, N>::rtensor(const shape_type& shape)
    {
        init_from_shape(shape);
    }

    /**
     * Allocates a rtensor with the specified shape and layout. Elements
     * are initialized to the specified value.
     * @param shape the shape of the rtensor
     * @param value the value of the elements
     * @param l the layout_type of the rtensor
     */
    template <class T, std::size_t N>
    inline rtensor<T, N>::rtensor(const shape_type& shape, const_reference value)
    {
        init_from_shape(shape);
        std::fill(m_data.begin(), m_data.end(), value);
    }
    //@}

    /**
     * @name Copy semantic
     */
    //@{
    /**
     * The copy constructor.
     */
    template <class T, std::size_t N>
    inline rtensor<T, N>::rtensor(const self_type& rhs)
    {
        init_from_shape(rhs.shape());
        std::copy(rhs.data().begin(), rhs.data().end(), this->data().begin());
    }

    /**
     * The assignment operator.
     */
    template <class T, std::size_t N>
    inline auto rtensor<T, N>::operator=(const self_type& rhs) -> self_type&
    {
        self_type tmp(rhs);
        *this = std::move(tmp);
        return *this;
    }
    //@}

    /**
     * @name Extended copy semantic
     */
    //@{
    /**
     * The extended copy constructor.
     */
    template <class T, std::size_t N>
    template <class E>
    inline rtensor<T, N>::rtensor(const xexpression<E>& e)
    {
        init_from_shape(e.derived_cast().shape());
        semantic_base::assign(e);
    }

    /**
     * The extended assignment operator.
     */
    template <class T, std::size_t N>
    template <class E>
    inline auto rtensor<T, N>::operator=(const xexpression<E>& e) -> self_type&
    {
        return semantic_base::operator=(e);
    }
    //@}

    template <class T, std::size_t N>
    template <class S>
    inline void rtensor<T, N>::reshape(const S& shape)
    {
        auto tmp = self_type::from_shape(shape);
        *static_cast<self_type*>(this) = std::move(tmp);
    }

    template <class T, std::size_t N>
    inline layout_type rtensor<T, N>::layout() const
    {
        return layout_type::column_major;
    }

    template <class T, std::size_t N>
    inline auto rtensor<T, N>::shape_impl() noexcept -> inner_shape_type&
    {
        return m_shape;
    }

    template <class T, std::size_t N>
    inline auto rtensor<T, N>::shape_impl() const noexcept -> const inner_shape_type&
    {
        return m_shape;
    }

    template <class T, std::size_t N>
    inline auto rtensor<T, N>::strides_impl() noexcept -> inner_strides_type&
    {
        return m_strides;
    }

    template <class T, std::size_t N>
    inline auto rtensor<T, N>::strides_impl() const noexcept -> const inner_strides_type&
    {
        return m_strides;
    }

    template <class T, std::size_t N>
    inline auto rtensor<T, N>::backstrides_impl() noexcept -> inner_backstrides_type&
    {
        return m_backstrides;
    }

    template <class T, std::size_t N>
    inline auto rtensor<T, N>::backstrides_impl() const noexcept -> const inner_backstrides_type&
    {
        return m_backstrides;
    }

    template <class T, std::size_t N>
    inline auto rtensor<T, N>::data_impl() noexcept -> container_type&
    {
        return m_data;
    }

    template <class T, std::size_t N>
    inline auto rtensor<T, N>::data_impl() const noexcept -> const container_type&
    {
        return m_data;
    }
}

namespace Rcpp
{
    template <typename T, std::size_t N>
    SEXP wrap(const xt::rtensor<T, N>& arr)
    {
        return SEXP(arr);
    }
}


#endif
