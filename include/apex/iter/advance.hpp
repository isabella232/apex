#ifndef APEX_ITER_ADVANCE_HPP
#define APEX_ITER_ADVANCE_HPP

#include <apex/core/iterator.hpp>
#include <apex/core/concepts.hpp>

namespace apex::detail::iter::advance {

template <class T> void advance (T&, iter_difference_t<T>) = delete;

struct function final {
  template <class T> requires requires { typename iter_difference_t<T>; }
  constexpr void operator () (T&& value, iter_difference_t<T> offset) const
  noexcept(noexcept(function::invoke(static_cast<T&&>(value), offset, prefer<2>))) {
    function::invoke(static_cast<T&&>(x), offset, prefer<2>);
  }
private:
  template <class T> requires (not ::std::is_lvalue_reference_v<T>)
  static constexpr void invoke (T&&, iter_difference_t<T>, preference<2>) = delete;

  template <class T>
  requires requires (T&& x, iter_difference_t<T> d) { static_cast<T&&>(x).advance(d); }
  static constexpr void invoke (T&& value, iter_difference_t<T> offset, preference<1>)
  noexcept(noexcept(static_cast<T&&>(value).advance(d))) {
    static_cast<T&&>(value).advance(offset);
  }

  template <class T>
  requires requires (T&& x, iter_difference_t<T> d) { advance(static_cast<T&&>(x), d); }
  static constexpr void invoke (T&& value, iter_difference_t<T> offset, preference<0>) 
  noexcept(noexcept(advance(static_cast<T&&>(value), offset))) {
    advance(static_cast<T&&>(value), offset);
  }
};

} /* namespace apex::detail::iter::advance */

namespace apex::iter {

/** @brief advance an iterable by some offset
 * Member versions of `advance` are called first, followed by ADL lookups of
 * advance.
 * @note rvalue references cannot be used with @c iter::advance
 * @ingroup cpo
 */
inline constexpr auto const advance = detail::iter::advance::function { };

} /* namespace apex::iter */

#endif /* APEX_ITER_ADVANCE_HPP */
