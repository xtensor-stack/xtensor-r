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

#include "xtensor_r_config.hpp"

#include <Rcpp.h>
#include <RcppCommon.h>

#include "rcpp_extensions.hpp"

// Fake type for R logicals
struct rlogical {};

// This handles the conversion of xtensor back to R type
// When an xtensor with a T of rlogical is seen, the retrieved rtype
// is LGLSXP (even though it is stored internally as an int in xtensor)
namespace Rcpp
{
    namespace traits
    {
        template<> struct r_sexptype_traits<rlogical>
        {
            static constexpr int rtype = LGLSXP;
        };
    }
}

namespace xt
{

    // R stores logicals as int32. This ensures that when a logical is seen,
    // the internal xtensor storage type uses an int.
    namespace r_detail
    {
        template <class T>
        struct get_underlying_value_type_r
        {
            using type = T;
        };

        template <>
        struct get_underlying_value_type_r<rlogical>
        {
            using type = int;
        };
    }

    namespace detail
    {
        inline xbuffer_adaptor<int*> r_shape_to_buffer_adaptor(SEXP exp)
        {
            if (Rf_isNull(Rf_getAttrib(exp, R_DimSymbol)))
            {
                // This is a 0D scalar
                if (Rf_xlength(exp) == 1)
                {
                    return xbuffer_adaptor<int*>(nullptr, 0);
                }

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
            if (n == 0)
            {
                return xbuffer_adaptor<int*>(nullptr, 0);
            }
            if (Rf_isNull(Rf_getAttrib(exp, R_DimSymbol)))
            {
                auto d = Rcpp::IntegerVector::create(Rf_length(exp));
                Rf_setAttrib(exp, R_DimSymbol, d);
            }

            SEXP shape_sexp = Rf_getAttrib(exp, R_DimSymbol);
            if (n != (std::size_t)Rf_xlength(shape_sexp))
            {
                throw std::runtime_error("Could not convert shape for rtensor. Dimensions don't match.");
            }
            return xbuffer_adaptor<int*>(
                Rcpp::internal::r_vector_start<INTSXP>(shape_sexp), n);
        }

        template <int SXP>
        inline void check_coercion(SEXP exp)
        {
        #if XTENSOR_WARN_ON_COERCE
            if (TYPEOF(exp) != SXP)
            {
                Rcpp::warning("Coerced object from '%s' to '%s'. Avoid for speed & in-place operations.",
                    Rf_type2char(TYPEOF(exp)),
                    Rf_type2char(SXP));
            }
        #endif
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
     * @tparam SP The Rcpp storage policy, defaults to Rcpp::PreserveStorage
     */
    template <class D, template <class> class SP = Rcpp::PreserveStorage>
    class rcontainer : public xcontainer<D>, public SP<D>
    {
    public:

        using derived_type = D;

        using rstorage = SP<D>;

        using base_type = xcontainer<D>;
        using inner_types = xcontainer_inner_types<D>;
        using r_type = typename inner_types::r_type;
        using storage_type = typename inner_types::storage_type;
        using value_type = typename storage_type::value_type;
        using reference = typename storage_type::reference;
        using const_reference = typename storage_type::const_reference;
        using pointer = typename storage_type::pointer;
        using const_pointer = typename storage_type::const_pointer;
        using size_type = typename storage_type::size_type;
        using difference_type = typename storage_type::difference_type;

#ifndef XTENSOR_R_ALLOW_REINTERPRETATION
        static_assert(xtl::disjunction<std::is_same<r_type, int32_t>,
                                       std::is_same<r_type, double>,
                                       std::is_same<r_type, Rbyte>,
                                       std::is_same<r_type, rlogical>,
                                       std::is_same<r_type, std::complex<double>>>::value == true,
                      "R containers can only be of type rlogical, int, double, std::complex<double>.");
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

        // explicitly forward data against ambiguity with Rcpp::Storage::data;
        using base_type::data;

    protected:

        rcontainer() = default;
        ~rcontainer() = default;

        rcontainer(const rcontainer&) = default;
        rcontainer& operator=(const rcontainer&) = default;

        rcontainer(rcontainer&&) = default;
        rcontainer& operator=(rcontainer&&) = default;

        // TODO: remove these once xcontainer::derived_cast is protected.
        derived_type& derived_cast() & noexcept;
        const derived_type& derived_cast() const & noexcept;
        derived_type derived_cast() && noexcept;
    };


    /**
     * Resizes the container.
     * @param shape the new shape
     */
    template <class D, template <class> class SP>
    template <class S>
    inline void rcontainer<D, SP>::resize(S&& shape)
    {
        // if SEXP not initialized, it will be NULL (e.g. in constructor)
        if (Rf_isNull(*this) || shape.size() != this->dimension() || !std::equal(std::begin(shape), std::end(shape), this->shape().cbegin()))
        {
            derived_type tmp(xtl::forward_sequence<shape_type, S>(shape));
            *static_cast<derived_type*>(this) = std::move(tmp);
        }
    }

    /**
     * Reshapes the container.
     * @param shape the new shape
     */
    template <class D, template <class> class SP>
    template <class S>
    inline void rcontainer<D, SP>::reshape(S&& shape)
    {
        if (compute_size(shape) != this->size())
        {
            throw std::runtime_error("Cannot reshape with incorrect number of elements.");
        }

        if (shape.size() != this->dimension() || !std::equal(std::begin(shape), std::end(shape), this->shape().cbegin()))
        {
            auto tmp_shape = Rcpp::IntegerVector(std::begin(shape), std::end(shape));
            Rf_setAttrib(rstorage::get__(), R_DimSymbol, SEXP(tmp_shape));
            this->derived_cast().update_shape_and_strides();
        }
    }

    template <class D, template <class> class SP>
    inline layout_type rcontainer<D, SP>::layout() const
    {
        return layout_type::column_major;
    }

    template <class D, template <class> class SP>
    inline auto rcontainer<D, SP>::derived_cast() & noexcept -> derived_type&
    {
        return *static_cast<derived_type*>(this);
    }

    template <class D, template <class> class SP>
    inline auto rcontainer<D, SP>::derived_cast() const & noexcept -> const derived_type&
    {
        return *static_cast<const derived_type*>(this);
    }

    template <class D, template <class> class SP>
    inline auto rcontainer<D, SP>::derived_cast() && noexcept -> derived_type
    {
        return *static_cast<derived_type*>(this);
    }
}

#endif
