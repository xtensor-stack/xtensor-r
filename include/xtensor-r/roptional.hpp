/***************************************************************************
* Copyright (c) 2016, Wolf Vollprecht, Johan Mabille and Sylvain Corlay    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XTENSOR_R_OPTIONAL_HPP
#define XTENSOR_R_OPTIONAL_HPP

#include <cstddef>
#include <type_traits>
#include <utility>

#include <xtl/xoptional.hpp>

#include <xtensor/xoptional_assembly.hpp>
#include <xtensor/xfunctor_view.hpp>

#include "rarray.hpp"
#include "rtensor.hpp"

namespace xt
{
    /***********************************
     * rcontainer_optional declaration *
     ***********************************/

    template <class RC>
    class rcontainer_optional;

    template <class T>
    using rarray_optional = rcontainer_optional<rarray<T>>;

    template <class T, std::size_t N>
    using rtensor_optional = rcontainer_optional<rtensor<T, N>>;

    /*
     * R uses special NaN values to represent missing floating values in arrays.
     *
     * rna_proxy is a proxy on the value with a boolean semantics, corresponding
     * to "has_value", i.e. `true` is the value is not NA, false otherwise.
     */
    template <class XT>
    struct rna_proxy
    {
        using cpp_type = std::decay_t<XT>;
        constexpr static int rtype = Rcpp::traits::r_sexptype_traits<cpp_type>::rtype;

        rna_proxy(XT val) : m_val(val)
        {
        }

        inline operator bool() const
        {
            return !Rcpp::traits::is_na<rtype>(m_val);
        }

        inline rna_proxy& operator=(bool val)
        {
            if (val == false)
            {
                m_val = Rcpp::traits::get_na<rtype>();
            }
            else if (m_val == Rcpp::traits::get_na<rtype>())
            {
                m_val = 0;
            }
            return *this;
        }

        XT m_val;
    };

    template <class XT>
    struct xproxy_inner_types<rna_proxy<XT>>
    {
        using proxy = rna_proxy<XT>;
        using reference = rna_proxy<XT>;
        using pointer = rna_proxy<XT>;
    };

    template <class T>
    struct rna_proxy_functor
    {
        using underlying_type = std::decay_t<T>;

        template <class xunderlying_type, class requested_type>
        using simd_return_type = xt_simd::simd_return_type<xunderlying_type, requested_type>;

        using cpp_type = std::decay_t<T>;
        constexpr static int rtype = Rcpp::traits::r_sexptype_traits<cpp_type>::rtype;

        using value_type = bool;
        using reference = rna_proxy<underlying_type&>;
        using const_reference = rna_proxy<const underlying_type&>;
        using pointer = rna_proxy<underlying_type*>;
        using const_pointer = rna_proxy<const underlying_type*>;

        reference operator()(T& val)
        {
            return val;
        }

        const_reference operator()(const T& val) const
        {
            return val;
        }

        // TODO: boolean loading in xsimd
        // template <class align, class requested_type, std::size_t N, class E>
        // auto proxy_simd_load(const E& expr, std::size_t n) const
        // {
        //     using simd_value_type = typename E::simd_value_type;
        //     return expr.template load_simd<align, requested_type, N>(n) == simd_value_type(NA_INTEGER);
        // }

        // template <class align, class simd, class E>
        // auto proxy_simd_store(E& expr, std::size_t n, const simd& batch) const
        // {
        //     using simd_value_type = typename E::simd_value_type;
        //     return expr.template store_simd<align>(n, xt_simd::select(batch, simd_value_type(0), simd_value_type(NA_INTEGER)));
        // }
    };

    template <class RC>
    struct xcontainer_inner_types<rcontainer_optional<RC>>
    {
        using raw_value_expression = RC;
        using value_type = typename raw_value_expression::value_type;
        using value_storage_type = typename raw_value_expression::storage_type&;
        using raw_flag_expression = xfunctor_adaptor<rna_proxy_functor<value_type>, RC&>;
        using flag_storage_type = xfunctor_adaptor<rna_proxy_functor<value_type>, RC&>;
        using storage_type = xoptional_assembly_storage<value_storage_type&, flag_storage_type&>;
        using temporary_type = rcontainer_optional<RC>;
    };

    template <class RC>
    struct xiterable_inner_types<rcontainer_optional<RC>>
    {
        using assembly_type = rcontainer_optional<RC>;
        using inner_shape_type = typename RC::inner_shape_type;
        using stepper = xoptional_assembly_stepper<assembly_type, false>;
        using const_stepper = xoptional_assembly_stepper<assembly_type, true>;
    };

    /**
     * @class rcontainer_optional
     * @brief Multidimensional container of optional values providing the xtensor
     * container semantics to an R array.
     *
     * ``rcontainer_optional`` is not meant to be used directly, but through the aliases
     * ``rarray_optional<T>`` and ``rtensor_optional<T, N>``.
     *
     * Depending on the value type, optional values are reference proxies on R's
     * ``NA_INTEGER``, ``NA_LOGICAL``, ``NA_REAL``, ``NA_STRING``, or ``R_NilValue``.
     *
     * Besides support for optionality, ``rarray_optional`` and ``rtensor_optional`` are similar
     * to the \ref rarray and \ref rtensor respectively, with respect to dynamic and static
     * dimensionality.
     *
     * @tparam T The type of the element stored in the rarray.
     */
    template <class RC>
    class rcontainer_optional : public xoptional_assembly_base<rcontainer_optional<RC>>,
                                public xcontainer_semantic<rcontainer_optional<RC>>
    {
    public:

        using self_type = rcontainer_optional<RC>;
        using base_type = xoptional_assembly_base<self_type>;
        using semantic_base = xcontainer_semantic<self_type>;

        using storage_type = typename base_type::storage_type;
        using assembly_type = rcontainer_optional<RC>;

        explicit rcontainer_optional(SEXP exp);

        rcontainer_optional(const rcontainer_optional&);
        rcontainer_optional& operator=(const rcontainer_optional&);
        rcontainer_optional(rcontainer_optional&&);
        rcontainer_optional& operator=(rcontainer_optional&&);

        template <class E>
        rcontainer_optional(const xexpression<E>& e);

        template <class E>
        rcontainer_optional& operator=(const xexpression<E>& e);
        
        operator SEXP() const noexcept;

    private:

        auto& value_impl() noexcept;
        const auto& value_impl() const noexcept;
        auto& has_value_impl() noexcept;
        const auto& has_value_impl() const noexcept;
        auto& storage_impl() noexcept;
        const auto& storage_impl() const noexcept;

        RC m_value;
        xfunctor_adaptor<rna_proxy_functor<typename RC::value_type>, RC&> m_flag;
        storage_type m_storage_proxy;

        friend xoptional_assembly_base<rcontainer_optional<RC>>;
    };

    /**************************************
     * rcontainer_optional implementation *
     **************************************/

    template <class RC>
    inline rcontainer_optional<RC>::rcontainer_optional(SEXP exp)
        : m_value(exp), m_flag(m_value), m_storage_proxy(m_value.storage(), m_flag)
    {
    }

    template <class RC>
    inline rcontainer_optional<RC>::rcontainer_optional(const rcontainer_optional& rhs)
        : m_value(rhs.m_value), m_flag(m_value), m_storage_proxy(m_value.storage(), m_flag)
    {
    }

    template <class RC>
    inline auto rcontainer_optional<RC>::operator=(const self_type& rhs) -> self_type&
    {
        base_type::operator=(rhs);
        m_value = rhs.m_value;
        return *this;
    }

    template <class RC>
    inline rcontainer_optional<RC>::rcontainer_optional(self_type&& rhs)
        : m_value(std::move(rhs.m_value)), m_flag(m_value), m_storage_proxy(m_value.storage(), m_flag)
    {
    }

    template <class RC>
    inline auto rcontainer_optional<RC>::operator=(self_type&& rhs) -> self_type&
    {
        base_type::operator=(rhs);
        m_value = std::move(rhs.m_value);
        return *this;
    }

    template <class RC>
    template <class E>
    inline rcontainer_optional<RC>::rcontainer_optional(const xexpression<E>& e)
        : m_value(), m_flag(m_value), m_storage_proxy(m_value.storage(), m_flag)
    {
        semantic_base::assign(e);
    }

    template <class RC>
    template <class E>
    inline auto rcontainer_optional<RC>::operator=(const xexpression<E>& e) -> self_type&
    {
        return semantic_base::operator=(e);
    }

    template <class RC>
    inline auto& rcontainer_optional<RC>::value_impl() noexcept
    {
        return m_value;
    }

    template <class RC>
    inline const auto& rcontainer_optional<RC>::value_impl() const noexcept
    {
        return m_value;
    }

    template <class RC>
    inline auto& rcontainer_optional<RC>::has_value_impl() noexcept
    {
        return m_flag;
    }

    template <class RC>
    inline const auto& rcontainer_optional<RC>::has_value_impl() const noexcept
    {
        return m_flag;
    }

    template <class RC>
    inline auto& rcontainer_optional<RC>::storage_impl() noexcept
    {
        return m_storage_proxy;
    }

    template <class RC>
    inline const auto& rcontainer_optional<RC>::storage_impl() const noexcept
    {
        return m_storage_proxy;
    }
    
    template <class RC>
    inline rcontainer_optional<RC>::operator SEXP() const noexcept
    {
        return SEXP(m_value);
    }
}

#endif

