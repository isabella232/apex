#ifndef APEX_MIXIN_HANDLE_HPP
#define APEX_MIXIN_HANDLE_HPP

#include <apex/core/concepts.hpp>
#include <apex/detect/operators.hpp>

#include <type_traits>

// TODO: consider renaming the type to resource_handle, or just 'resource'
// We can also use the name 'resource' for the concept itself. All of this is
// in flux.

namespace apex::mixin::impl {

template <class T> concept has_element_type = requires { typename T::element_type; };
template <class T> concept has_pointer = requires { typename T::pointer; };

template <class T> requires has_element_type<T>
std::add_pointer_t<typename T::element_type> pointer_type_of () noexcept;

template <class T> requires has_pointer<T>
typename T::pointer pointer_type_of () noexcept;

template <class T> using pointer_type_of_t = decltype(pointer_type_of<remove_cvref_t<T>>());

template <class T>
concept handle_storage = requires (T object) {
  requires equality_comparable_with<T, nullptr_t>;
  requires totally_ordered<T>;
  requires movable<T>;

  { object.get() } noexcept -> same_as<pointer_type_of_t<T>>;
  { static_cast<bool>(object) } noexcept;
  { object.reset(object.get()) } noexcept;
  { object.reset() } noexcept;
  { *object } noexcept -> same_as<decltype(*object.get())>;
};

} /* namespace apex::mixin::impl */

namespace apex::mixin {

// Intended to be used like:
/*
 * template <class T, class D=std::default_delete<T>>
 * using unique_handle = handle<T, std::unique_ptr<T, D>>;
 * struct my_type : protected unique_handle<int> { }
 */
// this saves a LOT of time and energy when wrapping C APIs, of which there are
// a great deal of in the world.
template <class T, class Storage>
struct handle {
  using storage_type = Storage;
  using pointer = detected_or_t<
    std::add_pointer_t<detected_or_t<T, detect::types::element_type, storage_type>>,
    detect::types::pointer,
    storage_type
  >;

  static_assert(std::is_default_constructible_v<storage_type>);
  static_assert(std::is_move_constructible_v<storage_type>);
//  static_assert(std::is_convertible_v<bool, storage_type>);
  static_assert(std::is_swappable_v<storage_type>);
  static_assert(is_detected_v<detect::pointer::dereference, storage_type>);
  static_assert(is_detected_v<detect::pointer::arrow, storage_type>);

  handle (std::nullptr_t) noexcept : handle { } { }
  handle () noexcept = default;

  explicit operator bool () const noexcept { return bool(this->storage); }
  decltype(auto) get () const noexcept { return this->storage.get(); }

  template <class... Args>
  void reset (pointer ptr, Args&&... args) noexcept {
    this->storage.reset(ptr, std::forward<Args>(args)...);
  }
  void reset () noexcept { this->storage.reset(); }

  void swap (handle& that) noexcept {
    using std::swap;
    swap(this->storage, that.storage);
  }
protected:
  template <class... Args> requires constructible_from<storage_type, pointer, Args...>
  handle (pointer ptr, Args&&... args)
    noexcept(std::is_nothrow_constructible_v<storage_type, pointer, Args...>) :
    storage { ptr, std::forward<Args>(args)... }
  { }
  storage_type storage;
};

} /* namespace apex::mixin */

#endif /* APEX_MIXIN_HANDLE_HPP */
