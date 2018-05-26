/***************************************************************************
* Copyright (c) 2016, Wolf Vollprecht, Johan Mabille and Sylvain Corlay    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XTENSOR_R_VECTORIZE_HPP
#define XTENSOR_R_VECTORIZE_HPP

#include <type_traits>

#include "rarray.hpp"
#include "xtensor/xvectorize.hpp"

namespace xt
{

    template <class Func, class R, class... Args>
    struct rvectorizer
    {
        xvectorizer<Func, R> m_vectorizer;

        template <class F, class = std::enable_if_t<!std::is_same<std::decay_t<F>, rvectorizer>::value>>
        rvectorizer(F&& func)
            : m_vectorizer(std::forward<F>(func))
        {
        }

        inline rarray<R> operator()(const rarray<Args>&... args) const
        {
            rarray<R> res = m_vectorizer(args...);
            return res;
        }
    };

    /**
     * @brief Create numpy-style universal function from scalar function.
     */
    template <class R, class... Args>
    inline rvectorizer<R (*)(Args...), R, Args...> rvectorize(R (*f)(Args...))
    {
        return rvectorizer<R (*)(Args...), R, Args...>(f);
    }

    /// @cond DOXYGEN_INCLUDE_OVERLOADS
    template <class F, class R, class... Args>
    inline rvectorizer<F, R, Args...> rvectorize(F&& f, R (*)(Args...))
    {
        return rvectorizer<F, R, Args...>(std::forward<F>(f));
    }

    template <class F>
    inline auto rvectorize(F&& f) -> decltype(rvectorize(std::forward<F>(f), (detail::get_function_type<F>*)nullptr))
    {
        return rvectorize(std::forward<F>(f), (detail::get_function_type<F>*)nullptr);
    }
    /// @endcond
}

#endif
