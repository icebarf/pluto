/*
 * NEG / NEGB tests.
 *
 * Handbook reference: page 45.
 *   N/Z from result.
 *   V set if result is 100000 word (200 byte).
 *   C cleared if result is 0; set otherwise.
 */
#include <array>

#include "test_framework.hpp"

using namespace test;
using enum Instructions::Mode;

namespace {

struct NegWordCases
{
  using Op = op::Neg;
  static constexpr bool is_byte = false;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "neg_word_example", 0000010, 0177770, {}, flag::n | flag::c },
    { "neg_word_zero", 0, 0, {}, flag::z },
    { "neg_word_two", 2, 0177776, {}, flag::n | flag::c },
    { "neg_word_most_negative", 0100000, 0100000, {}, flag::n | flag::v | flag::c },
  });
};
REGISTER_SUITE(NegWordCases);

struct NegByteCases
{
  using Op = op::Neg;
  static constexpr bool is_byte = true;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "neg_byte_register_preserves_high_byte", 012001, 012377,
      {}, flag::n | flag::c },
    { "neg_byte_zero", 012000, 012000, {}, flag::z },
    { "neg_byte_most_negative", 012200, 012200, {}, flag::n | flag::v | flag::c },
  });
};
REGISTER_SUITE(NegByteCases);

} // namespace

TEST(neg_word_autoincrement)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_word(DATA, 1);

  t.execute<op::Neg>(AutoIncrement, 0);

  t.expect_word(DATA, 0177777);
  t.expect_reg(0, DATA + 2);
  t.expect_flags(flag::n | flag::c);
}
