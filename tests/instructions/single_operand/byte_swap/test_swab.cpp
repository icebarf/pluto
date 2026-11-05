/*
 * SWAB tests.
 *
 * Handbook reference: page 57.
 *   Swap the high and low bytes.
 *   N: set if bit 7 of the low-order byte of the result is set.
 *   Z: set if the low-order byte of the result is zero.
 *   V: cleared. C: cleared.
 */
#include <array>

#include "test_framework.hpp"

using namespace test;
using enum Instructions::Mode;

namespace {

struct SwabCases
{
  using Op = op::Swab;
  static constexpr bool is_byte = false;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "swab_example", 0077777, 0177577, {}, {} },
    { "swab_zero", 0, 0, {}, flag::z },
    { "swab_zero_low_byte_with_nonzero_high_byte", 0000012, 005000, {}, flag::z },
    { "swab_negative_from_low_byte", 0100000, 0000200, {}, flag::n },
    { "swab_clears_v_and_c", 012345, 0162424, flag::all, {} },
  });
};
REGISTER_SUITE(SwabCases);

} // namespace

TEST(swab_memory_word)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_word(DATA, 0077777);

  t.execute<op::Swab>(RegisterDeferred, 0);

  t.expect_word(DATA, 0177577);
  t.expect_reg(0, DATA);
  t.expect_flags(flag::none);
}
