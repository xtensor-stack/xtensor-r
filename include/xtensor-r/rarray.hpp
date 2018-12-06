/***************************************************************************
* Copyright (c) 2017, Wolf Vollprecht, Johan Mabille, and Sylvain Corlay   *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XTENSOR_R_ARRAY_HPP
#define XTENSOR_R_ARRAY_HPP

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "xtensor/xbuffer_adaptor.hpp"
#include "xtensor/xcontainer.hpp"
#include "xtensor/xiterator.hpp"
#include "xtensor/xsemantic.hpp"
#include "xtensor/xutils.hpp"

#include "xtensor-r/rcontainer.hpp"

namespace xt
{
    template <class T>
    class rarray;

    template <class T>
    struct xcontainer_inner_types<rarray<T>>
    {
        using r_type = T;
        using underlying_type = r_detail::get_underlying_value_type_r<T>;
        using storage_type = xbuffer_adaptor<typename underlying_type::type*>;
        constexpr static int SXP = Rcpp::traits::r_sexptype_traits<T>::rtype;
        using shape_type = xt::dynamic_shape<typename storage_type::size_type>;
        using strides_type = xt::dynamic_shape<typename storage_type::difference_type>;
        using backstrides_type = xt::dynamic_shape<typename storage_type::difference_type>;

        using shape_value_type = typename Rcpp::traits::storage_type<INTSXP>::type;
        using inner_shape_type = xbuffer_adaptor<shape_value_type*>;
        using inner_strides_type = strides_type;
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

        using underlying_type = typename inner_types::underlying_type;
        using storage_type = typename inner_types::storage_type;
        using value_type = typename storage_type::value_type;
        using reference = typename storage_type::reference;
        using const_reference = typename storage_type::const_reference;
        using pointer = typename storage_type::pointer;
        using const_pointer = typename storage_type::const_pointer;
        using size_type = typename storage_type::size_type;
        using difference_type = typename storage_type::difference_type;

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

        constexpr static int SXP = Rcpp::traits::r_sexptype_traits<T>::rtype;

        rarray() = default;
        rarray(SEXP exp);

        rarray(const shape_type& shape);
        rarray(const shape_type& shape, const_reference value);

        rarray(const value_type& t);
        rarray(nested_initializer_list_t<value_type, 1> t);
        rarray(nested_initializer_list_t<value_type, 2> t);
        rarray(nested_initializer_list_t<value_type, 3> t);
        rarray(nested_initializer_list_t<value_type, 4> t);
        rarray(nested_initializer_list_t<value_type, 5> t);

        template <class S = shape_type>
        static rarray from_shape(S&& shape);

        template <class E>
        rarray(const xexpression<E>& e);

        rarray(const self_type& rhs);
        self_type& operator=(const self_type& rhs);

        rarray(self_type&&) = default;
        self_type& operator=(self_type&&) = default;

        template <class E>
        self_type& operator=(const xexpression<E>& e);

        layout_type layout() const;

        // Internal method, called when the stored SEXP changes
        void update(SEXP new_sexp) noexcept;

    private:

        storage_type m_storage;
        inner_shape_type m_shape;
        strides_type m_strides;
        strides_type m_backstrides;

        template <class S>
        void init_from_shape(const S& shape);

        const inner_shape_type& shape_impl() const noexcept;
        const inner_strides_type& strides_impl() const noexcept;
        const inner_backstrides_type& backstrides_impl() const noexcept;
        storage_type& storage_impl() noexcept;
        const storage_type& storage_impl() const noexcept;

        void update_shape_and_strides() noexcept;

        friend class xcontainer<rarray<T>>;
        friend class rcontainer<rarray<T>, Rcpp::PreserveStorage>;
    };

    template <class T>
    inline rarray<T>::rarray(SEXP exp)
    {
        detail::check_coercion<SXP>(exp);
        base_type::rstorage::set__(Rcpp::r_cast<SXP>(exp));
    }

    template <class T>
    template <class S>
    inline void rarray<T>::init_from_shape(const S& shape)
    {
        auto tmp_shape = Rcpp::IntegerVector(shape.begin(), shape.end());
        base_type::rstorage::set__(Rf_allocArray(SXP, SEXP(tmp_shape)));
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
        : base_type(rhs), semantic_base(rhs)
    {
        init_from_shape(rhs.shape());
        std::copy(rhs.storage().cbegin(), rhs.storage().cend(), this->storage().begin());
    }

    template <class T>
    inline rarray<T>::rarray(const value_type& t)
    {
        init_from_shape(xt::shape<shape_type>(t));
        nested_copy(m_storage.begin(), t);
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
    template <class S>
    inline rarray<T> rarray<T>::from_shape(S&& shape)
    {
        return self_type(xtl::forward_sequence<shape_type, S>(shape));
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
    inline auto rarray<T>::storage_impl() noexcept -> storage_type&
    {
        return m_storage;
    }

    template <class T>
    inline auto rarray<T>::storage_impl() const noexcept -> const storage_type&
    {
        return m_storage;
    }

    template <class T>
    inline void rarray<T>::update(SEXP new_sexp) noexcept
    {
        this->m_shape = detail::r_shape_to_buffer_adaptor(new_sexp);
        resize_container(m_strides, m_shape.size());
        resize_container(m_backstrides, m_shape.size());
        std::size_t sz = xt::compute_strides<layout_type::column_major>(m_shape, layout_type::column_major, m_strides, m_backstrides);
        this->m_storage = storage_type(reinterpret_cast<value_type*>(Rcpp::internal::r_vector_start<SXP>(new_sexp)), sz);
    }

    template <class T>
    inline void rarray<T>::update_shape_and_strides() noexcept
    {
        m_shape = detail::r_shape_to_buffer_adaptor(*this);
        xt::compute_strides<layout_type::column_major>(m_shape, layout_type::column_major, m_strides, m_backstrides);
    }
}

namespace Rcpp
{
    template <typename T>
    inline SEXP wrap(const xt::rarray<T>& arr)
    {
        return SEXP(arr);
    }
}

#endif
