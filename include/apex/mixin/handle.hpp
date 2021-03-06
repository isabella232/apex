#ifndef APEX_MIXIN_HANDLE_HPP
#define APEX_MIXIN_HANDLE_HPP

#include <apex/core/concepts.hpp>
#include <apex/core/meta.hpp>

// TODO: consider renaming the type to resource_handle, or just 'resource'
// We can also use the name 'resource' for the concept itself. All of this is
// in flux.
namespace apex::mixin::detail {

template <class T>
concept has_element_type = requires { typename T::element_type; };

template <class T>
concept has_pointer = requires { typename T::pointer; };

template <class T>
concept element_only = has_element_type<T> and not has_pointer<T>;

template <class T>
concept pointer_only = not has_element_type<T> and has_pointer<T>;

template <class T>
concept element_and_pointer = has_element_type<T> and has_pointer<T>;

template <class> struct pointer_type_of;

template <element_only T>
struct pointer_type_of<T> :
  ::std::add_pointer_t<typename T::element_type>
{ };

template <pointer_only T>
struct pointer_type_of<T> :
  type_identity<typename T::pointer>
{ };

template <element_and_pointer T>
struct pointer_type_of<T> :
  type_identity<typename T::pointer>
{ };

template <class T>
using pointer_type_of_t = typename pointer_type_of<T>::type;

template <class T>
concept handle_storage = requires (T object) {
  //requires equality_comparable_with<T, nullptr_t>;
  requires totally_ordered<T>;
  requires swappable<T>;
  requires movable<T>;

  typename pointer_type_of_t<T>;

  { object.get() } noexcept -> same_as<pointer_type_of_t<T>>;
  object.operator->();
  static_cast<bool>(object);
  object.reset(object.get());
  object.reset();
};

template <class T>
concept releasable = requires (T object) {
  { object.release() } -> same_as<typename pointer_type_of<T>::type>;
};

} /* namespace apex::mixin::detail */

namespace apex::mixin {

/** @brief Resource handle mixin
 * This is used to save a *lot* of time and energy when wrapping C APIs, of
 * which there are a great deal in the world.
 *
 * An example of intended usage:
 * @code{.cpp}
 * template <class T, class D=std::default_delete<T>>
 * using unique_handle = handle<T, std::unique_ptr<T, D>>;
 * struct my_type : protected unique_handle<int> { };
 * @endcode
 *
 * @note This should ONLY be used to wrapping object lifetimes when other types
 * do not follow RAII. This applies to both C and C++ APIs
 */
template <class T, detail::handle_storage Storage>
struct [[deprecated("Please use apex::mixin::resource instead")]] handle {
  using storage_type = Storage;
  using pointer = detail::pointer_type_of_t<remove_cvref_t<storage_type>>;
  using handle_type = handle;

  handle (std::nullptr_t) noexcept :
    handle { }
  { }
  handle () noexcept = default;

  explicit operator bool () const noexcept { return bool(this->storage); }
  decltype(auto) get () const noexcept { return this->storage.get(); }

  template <class... Args>
  void reset (pointer ptr, Args&&... args) noexcept {
    this->storage.reset(ptr, std::forward<Args>(args)...);
  }
  void reset () noexcept { this->storage.reset(); }

  pointer release () noexcept requires detail::releasable<storage_type> {
    return this->storage.release();
  }

  void swap (handle& that) noexcept {
    using std::swap;
    swap(this->storage, that.storage);
  }

  template <class... Args> requires constructible_from<storage_type, pointer, Args...>
  explicit (not sizeof...(Args)) handle (pointer ptr, Args&&... args)
    noexcept(is_nothrow_constructible_v<storage_type, pointer, Args...>) :
    storage { ptr, std::forward<Args>(args)... }
  { }
protected:
  storage_type storage;
};

} /* namespace apex::mixin */

#endif /* APEX_MIXIN_HANDLE_HPP */
