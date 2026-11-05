/*
 * COM / COMB tests.
 *
 * Handbook reference: page 47.
 *   N/Z from result, V cleared, C set.
 *   Byte: only the low byte is complemented; register high byte preserved.
 */
#include <array>

#include "test_framework.hpp"

using namespace test;
using enum Instructions::Mode;

namespace {

struct ComWordCases
{
  using Op = op::Com;
  static constexpr bool is_byte = false;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "com_word_example", 013333, 0164444, {}, flag::n | flag::c },
    { "com_word_zero", 0, 0177777, {}, flag::n | flag::c },
    { "com_word_all_ones", 0177777, 0, {}, flag::z | flag::c },
  });
};
REGISTER_SUITE(ComWordCases);

struct ComByteCases
{
  using Op = op::Com;
  static constexpr bool is_byte = true;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "com_byte_preserves_high_byte", 0022222, 0022155, {}, flag::c },
    { "com_byte_zero", 0000377, 0, {}, flag::z | flag::c },
  });
};
REGISTER_SUITE(ComByteCases);

} // namespace

TEST(com_word_memory)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_word(DATA, 013333);

  t.execute<op::Com>(RegisterDeferred, 0);

  t.expect_word(DATA, 0164444);
  t.expect_reg(0, DATA);
  t.expect_flags(flag::n | flag::c);
}

TEST(com_byte_memory_does_not_touch_neighbor)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_byte(DATA, 0xab);
  t.set_byte(DATA + 1, 0xcd);

  t.execute<op::Com, true>(RegisterDeferred, 0);

  t.expect_byte(DATA, 0x54);
  t.expect_byte(DATA + 1, 0xcd);
  t.expect_flags(flag::c);
}
