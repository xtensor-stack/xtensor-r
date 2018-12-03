/***************************************************************************
* Copyright (c) 2016, Wolf Vollprecht, Johan Mabille and Sylvain Corlay    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XTENSOR_R_OPTIONAL_HPP
#define XTENSOR_R_OPTIONAL_HPP

#include "../../xtl/include/xtl/xoptional.hpp"
#include "../../xtensor/include/xtensor/xoptional_assembly.hpp"
#include <xtensor/xfunctor_view.hpp>

#include "rarray.hpp"

namespace xt
{

    template <class XT>
    struct rna_proxy
    {
        using cpp_type = std::decay_t<XT>;
        constexpr static int rtype = Rcpp::traits::r_sexptype_traits<cpp_type>::rtype;

        rna_proxy(XT val) : m_val(val) {};

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
        using simd_return_type = xsimd::simd_return_type<xunderlying_type, requested_type>;

        using cpp_type = std::decay_t<T>;
        constexpr static int rtype = Rcpp::traits::r_sexptype_traits<cpp_type>::rtype;

        using value_type = bool;
        using reference = rna_proxy<underlying_type&>;
        using const_reference = rna_proxy<const underlying_type&>;
        using pointer = rna_proxy<underlying_type*>;
        using const_pointer = rna_proxy<const underlying_type*>;

        reference operator()(T& val) {
            return val;
        }

        const_reference operator()(const T& val) const {
            return val;
        }

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
        //     return expr.template store_simd<align>(n, xsimd::select(batch, simd_value_type(0), simd_value_type(NA_INTEGER)));
        // }
    };

    template <class T>
    class roptional_assembly;

    template <class T>
    struct xcontainer_inner_types<roptional_assembly<T>>
    {
        using raw_value_expression = rarray<T>;
        using value_storage_type = typename raw_value_expression::storage_type&;
        using raw_flag_expression = xfunctor_view<rna_proxy_functor<T>, rarray<T>&>;
        using flag_storage_type = xfunctor_view<rna_proxy_functor<T>, rarray<T>&>;
        using storage_type = xoptional_assembly_storage<value_storage_type&, flag_storage_type&>;
        using temporary_type = roptional_assembly<T>;
    };

    template <class T>
    struct xiterable_inner_types<roptional_assembly<T>>
    {
        using assembly_type = roptional_assembly<T>;
        using inner_shape_type = typename rarray<T>::inner_shape_type;
        using stepper = xoptional_assembly_stepper<assembly_type, false>;
        using const_stepper = xoptional_assembly_stepper<assembly_type, true>;
    };

    template <class T>
    class roptional_assembly : public xoptional_assembly_base<roptional_assembly<T>>,
                               public xcontainer_semantic<roptional_assembly<T>>
    {
    public:
        using base_type = xoptional_assembly_base<roptional_assembly<T>>;
        using storage_type = typename base_type::storage_type;
        using assembly_type = roptional_assembly<T>;

        roptional_assembly(SEXP exp)
            : m_value(exp), m_flag(m_value), m_storage_proxy(m_value.storage(), m_flag)
        {
        }

    protected:

        auto& value_impl() {
            return m_value;
        }

        const auto& value_impl() const {
            return m_value;
        }

        auto& has_value_impl() {
            return m_flag;
        }

        const auto& has_value_impl() const {
            return m_flag;
        }

        auto& storage_impl() {
            return m_storage_proxy;
        }

        const auto& storage_impl() const {
            return m_storage_proxy;
        }

    private:
        rarray<T> m_value;
        xfunctor_view<rna_proxy_functor<T>, rarray<T>&> m_flag;
        storage_type m_storage_proxy;

        friend xoptional_assembly_base<roptional_assembly<T>>;
    };
}

namespace Rcpp
{
    namespace traits
    {
        template <class T>
        class Exporter<xt::roptional_assembly<T>>
        {

        public:
            Exporter(SEXP x)
                : m_sexp(x)
            {
            }

            inline xt::roptional_assembly<T> get()
            {
                return m_sexp;
            }

        private:
            SEXP m_sexp;
        };
    }
}

#endif