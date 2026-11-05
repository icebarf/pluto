/*
 * TST / TSTB tests.
 *
 * Handbook reference: page 46.
 *   N/Z from the destination, V and C cleared.
 *   The destination is not modified.
 */
#include <array>

#include "test_framework.hpp"

using namespace test;
using enum Instructions::Mode;

namespace {

struct TstWordCases
{
  using Op = op::Tst;
  static constexpr bool is_byte = false;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "tst_word_positive", 0000333, 0000333, flag::all, {} },
    { "tst_word_zero", 0, 0, flag::all, flag::z },
    { "tst_word_negative", 0100000, 0100000, {}, flag::n },
  });
};
REGISTER_SUITE(TstWordCases);

struct TstByteCases
{
  using Op = op::Tst;
  static constexpr bool is_byte = true;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "tst_byte_negative", 0000200, 0000200, {}, flag::n },
    { "tst_byte_zero_ignores_high_byte", 012000, 012000, {}, flag::z },
  });
};
REGISTER_SUITE(TstByteCases);

} // namespace

TEST(tst_memory_is_not_modified)
{
  t.reset(flag::all);
  t.reg(0) = DATA;
  t.set_word(DATA, 012345);

  t.execute<op::Tst>(RegisterDeferred, 0);

  t.expect_word(DATA, 012345);
  t.expect_reg(0, DATA);
  t.expect_flags(flag::none);
}
