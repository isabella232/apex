#ifndef APEX_CONCURRENCY_HPP
#define APEX_CONCURRENCY_HPP

#include <functional>
#include <mutex>

namespace apex {
inline namespace v1 {

// TODO: implement a thread type that works much like std::thread, but also
// supports operations like the proposed std::thread::attributes (and other
// pthread related operations). Might also be worth looking into adding a
// threadpool, or just wrapping grand central dispatch instead.

// Still needs some work, but this is effectively based off of the proposed
// synchronized_value
template <class T, class U=std::mutex>
struct synchronized final {
  using value_type = T;
  using mutex_type = U;

  template <Args...>
  synchronized (Args&&... args) noexcept(NothrowConstructible<T, Args...>) :
    val { std::forward<Args>(args)... }
  { }
  ~synchronized () = default;

  template <class F, class... Args>
  friend decltype(auto) apply (F&& f, synchronized<Args>&... values) {
    std::scoped_lock(values.mtx...);
    return std::invoke(std::forward<F>(f), values.val...);
  }
private:
  mutable mutex_type mtx;
  value_type val;
};



}} /* namespace apex::v1 */

#endif /* APEX_CONCURRENCY_HPP */
