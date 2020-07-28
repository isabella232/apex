#ifndef APEX_ITER_PREV_HPP
#define APEX_ITER_PREV_HPP

#include <apex/iter/advance.hpp>

namespace apex::detail::iter::next {

template <class T> void prev (T&) = delete;

struct function final {
private:
  template <class T> requires (not ::std::is_lvalue_reference_v<T>)
  static constexpr void invoke (T&&, preference<3>) = delete;

  template <class T>
  requires requires (T&& x) { static_cast<T&&>(x).prev(); }
  static constexpr void invoke (T&& value, preference<2>)
  noexcept(noexcept(static_cast<T&&>(value).prev())) {
    static_cast<T&&>(value).prev();
  }

  template <class T>
  requires requires (T&& x) { prev(static_cast<T&&>(x)); }
  static constexpr void invoke (T&& value, preference<1>)
  noexcept(noexcept(prev(static_cast<T&&>(value)))) {
    prev(static_cast<T&&>(value));
  }

  template <class T>
  requires requires (T&& x) { ::apex::iter::advance(static_cast<T&&>(x), -1); }
  static constexpr void invoke (T&& value, preference<0>)
  noexcept(noexcept(::apex::iter::advance(static_cast<T&&>(value), -1))) {
    ::apex::iter::advance(static_cast<T&&>(value), -1);
  }
};

} /* namespace apex::detail::iter::next */

namespace apex::iter {

inline constexpr auto const prev = detail::iter::next::function { };

} /* namespace apex::iter */

#endif /* APEX_ITER_PREV_HPP */
