#ifndef APEX_CORE_MEMORY_HPP
#define APEX_CORE_MEMORY_HPP

#include <apex/core/concepts.hpp>
#include <memory>
#include <new>

// Like most core/ files, this provides a shim for various stdlib items

namespace apex {

#if APEX_CHECK_API(to_address, 201711)
using ::std::to_address;
#else
template <class T> requires requires (T const& p) { p.operator->(); }
constexpr auto to_address (T const& p) noexcept { return p.operator->(); }
template <class T> requires (not is_function_v<T>)
constexpr auto to_address (T* p) noexcept { return p; }
#endif /* APEX_CHECK_API(to_address, 201711) */

#if APEX_CHECK_API(constexpr_dynamic_alloc, 201907)
using ::std::construct_at;
using ::std::destroy_at;
#else
template <class T>
void destroy_at (T* ptr) noexcept(is_nothrow_destructible_v<T>) {
  if constexpr (std::is_array_v<T>) {
    for (auto& element : *ptr) {
      apex::destroy_at(std::addressof(element));
    }
  } else if constexpr (not std::is_trivially_destructible_v<T>) {
    ptr->~T();
  }
}

template <class T, class... Args>
requires constructible_from<T, Args...>
auto construct_at (T* location, Args&&... args) noexcept(is_nothrow_constructible_v<T, Args...>) {
  if constexpr (std::is_aggregate_v<T>) {
    return ::new (static_cast<void*>(location)) T { std::forward<Args>(args)... };
  } else {
    return ::new (static_cast<void*>(location)) T(std::forward<Args>(args)...);
  }
}
#endif /* APEX_CHECK_API(constexpr_dynamic_alloc, 201907) */

} /* namespace apex */

#endif /* APEX_CORE_MEMORY_HPP */
