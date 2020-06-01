#ifndef APEX_CORE_TYPES_HPP
#define APEX_CORE_TYPES_HPP

#include <cstddef>
#include <cstdint>

namespace apex {

using std::nullptr_t;
using std::ptrdiff_t;
using std::size_t;
using std::byte;

using czstr = char const*;
using zstr = char*;

using i64 = std::int64_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using i8 = std::int8_t;

using u64 = std::uint64_t;
using u32 = std::uint32_t;
using u16 = std::uint16_t;
using u8 = std::uint8_t;

using f64 = double;
using f32 = float;

struct empty { };

} /* namespace apex */

// Some 'default' types
namespace apex::defaults {

struct difference_type_t { using difference_type = ptrdiff_t; };

} /* namespace apex::defaults */

#endif /* APEX_CORE_TYPES_HPP */
