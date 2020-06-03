#ifndef APEX_CORE_OPTIONAL_HPP
#define APEX_CORE_OPTIONAL_HPP

#include <apex/memory/memory.hpp>

namespace apex {

// TODO: finish adding constructors
// TODO: Add assignment, swap, std API, monadic operations, etc.
// TODO: Add optional<bool> that is size effecient (i.e., sizeof(bool))

template <class T>
struct optional final {
  using value_type = T;

  template <class... Args>
  requires constructible_from<T, Args...>
  optional (in_place_t, Args&&... args) noexcept(is_nothrow_constructible_v<T, Args...>) :
    valid(true)
  { apex::construct_at(std::addressof(this->value), std::forward<Args>(args)...); }

  optional (optional const&) = default;

  void swap (optional& that) noexcept(std::is_nothrow_swappable<T>) {
    using std::swap;
    if (not *this and not that) { return; }
    if (*this and *that) { swap(**this, *that); }
  }

  explicit operator bool () const noexcept { return this->valid; }

  decltype(auto) operator * () const noexcept { return *this->value; }
  auto operator -> () const noexcept { return std::addressof(this->value); }

  void reset () noexcept {
    if (*this) {
      destroy_at(this->value);
      this->valid = false;
    }
  }

  template <class... Args>
  requires constructible_from<Args...>
  auto emplace (Args&&... args) noexcept(is_nothrow_constructible_v<Args...>) {
    if (*this) { apex::destroy_at(std::addressof(this->value)); }
    apex::construct_at(std::addressof(this->value), std::forward<Args>(args)...);
  }

private:
  union { value_type value; };
  bool valid;
};

template <class T>
struct optional<T&> final {
  using storage_type = std::add_pointer_t<std::remove_reference_t<T>>;
  using value_type = T&;

  explicit optional (T& ref) noexcept :
    value(std::addressof(ref))
  { }

  optional (in_place_t, T& ref) noexcept :
    optional(ref)
  { }

  optional (std::reference_wrapper<T> ref) noexcept :
    optional(ref.get())
  { }

  optional (T&&) = delete;

  optional (optional const&) = default;
  optional () noexcept = default;

  optional& operator = (optional const&) = default;

  explicit operator bool () const noexcept { return this->value; }

  decltype(auto) operator * () const noexcept { return *this->value; }
  auto operator -> () const noexcept { return this->value; }

private:
  storage_type value { };
};

template <class T> optional(T) -> optional<T>;

} /* namespace apex */

#endif /* APEX_CORE_OPTIONAL_HPP */
