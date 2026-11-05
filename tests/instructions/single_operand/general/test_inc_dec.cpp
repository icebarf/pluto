/*
 * INC / INCB / DEC / DECB tests.
 *
 * Handbook references:
 *   INC  page 44: N/Z result, V if (dst) held 077777 (0177 byte), C unaffected.
 *   DEC  page 43: N/Z result, V if (dst) was 100000 (0200 byte), C unaffected.
 */
#include <array>

#include "test_framework.hpp"

using namespace test;
using enum Instructions::Mode;

namespace {

struct IncWordCases
{
  using Op = op::Inc;
  static constexpr bool is_byte = false;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "inc_word_register", 0000333, 0000334, flag::c, flag::c },
    { "inc_word_overflow", 0077777, 0100000, {}, flag::n | flag::v },
    { "inc_word_wrap", 0177777, 0, {}, flag::z },
  });
};
REGISTER_SUITE(IncWordCases);

struct IncByteCases
{
  using Op = op::Inc;
  static constexpr bool is_byte = true;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "inc_byte_register_preserves_high_byte", 012377, 012000,
      flag::c, flag::z | flag::c },
    { "inc_byte_overflow", 012177, 012200, {}, flag::n | flag::v },
  });
};
REGISTER_SUITE(IncByteCases);

struct DecWordCases
{
  using Op = op::Dec;
  static constexpr bool is_byte = false;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "dec_word_register", 1, 0, flag::c, flag::z | flag::c },
    { "dec_word_overflow", 0100000, 0077777, {}, flag::v },
    { "dec_word_wrap", 0, 0177777, {}, flag::n },
  });
};
REGISTER_SUITE(DecWordCases);

struct DecByteCases
{
  using Op = op::Dec;
  static constexpr bool is_byte = true;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "dec_byte_register_preserves_high_byte", 012300, 012277,
      flag::c, flag::n | flag::c },
    { "dec_byte_overflow", 012200, 012177, {}, flag::v },
  });
};
REGISTER_SUITE(DecByteCases);

} // namespace

TEST(inc_word_autoincrement)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_word(DATA, 5);

  t.execute<op::Inc>(AutoIncrement, 0);

  t.expect_word(DATA, 6);
  t.expect_reg(0, DATA + 2);
  t.expect_pc(01002);
}

TEST(inc_byte_autoincrement_steps_one)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_byte(DATA, 0xff);

  t.execute<op::Inc, true>(AutoIncrement, 0);

  t.expect_byte(DATA, 0);
  t.expect_reg(0, DATA + 1);
  t.expect_flags(flag::z);
}

TEST(inc_word_autodecrement)
{
  t.reset();
  t.reg(0) = DATA + 2;
  t.set_word(DATA, 5);

  t.execute<op::Inc>(AutoDecrement, 0);

  t.expect_word(DATA, 6);
  t.expect_reg(0, DATA);
}

TEST(inc_word_index)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_word(DATA + 4, 5);

  t.execute<op::Inc>(Index, 0, 4);

  t.expect_word(DATA + 4, 6);
  t.expect_reg(0, DATA);
  t.expect_pc(01004);
}

TEST(dec_word_autoincrement)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_word(DATA, 5);

  t.execute<op::Dec>(AutoIncrement, 0);

  t.expect_word(DATA, 4);
  t.expect_reg(0, DATA + 2);
}

TEST(dec_byte_autodecrement_steps_one)
{
  t.reset();
  t.reg(0) = DATA + 1;
  t.set_byte(DATA, 1);

  t.execute<op::Dec, true>(AutoDecrement, 0);

  t.expect_byte(DATA, 0);
  t.expect_reg(0, DATA);
  t.expect_flags(flag::z);
}
