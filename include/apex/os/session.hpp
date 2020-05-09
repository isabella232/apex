#ifndef APEX_OS_SESSION_HPP
#define APEX_OS_SESSION_HPP

#include <string_view>
#include <iterator>

#include <cstddef>

// slightly modified implementation of P1275

namespace apex::os {

struct environment final {

};

// TODO: Add ssize() call
// TODO: Permit constructing *with* an argc, argv** pair
// TODO: Add string_view-like operations to reduce size/slice.
struct arguments final {
  using iterator = char const* const*; // temporary for now, will eventually be a string view :)
  using reverse_iterator = std::reverse_iterator<iterator>;
  using value_type = std::string_view;
  using size_type = size_t;

  arguments (int, char const* const*) noexcept;
  arguments () noexcept = delete;

  [[nodiscard]] bool empty() const noexcept;
  size_type size () const noexcept;

private:
  char const* const* argv;
  int argc;
};

arguments const& args () noexcept;

} /* namespace apex::os */

#endif /* APEX_OS_SESSION_HPP */
