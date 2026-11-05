/*
 * BIS/BISB, BIT/BITB and BIC/BICB tests.
 *
 * Handbook pages 65-67:
 *   BIS: dst <- dst | src; BIT: flags from dst & src, no write;
 *   BIC: dst <- dst & ~src.
 *   N/Z from result, V cleared, C unaffected.
 */
#include "test_framework.hpp"

using namespace test;
using enum Instructions::Mode;

TEST(bis_word_register)
{
  t.reset(flag::c);
  t.reg(0) = 001234;
  t.reg(1) = 001111;

  t.execute_double<op::Bis>(Register, 0, Register, 1);

  t.expect_reg(1, 001335);
  t.expect_flags(flag::c);
}

TEST(bisb_register_preserves_high_byte)
{
  t.reset(flag::c);
  t.reg(0) = 0x0034;
  t.reg(1) = 0xabcd;

  t.execute_double<op::Bis, true>(Register, 0, Register, 1);

  t.expect_reg(1, 0xabfd);
  t.expect_flags(flag::n | flag::c);
}

TEST(bis_word_memory_destination)
{
  t.reset();
  t.reg(0) = 0000010;
  t.reg(1) = DATA;
  t.set_word(DATA, 0000020);

  t.execute_double<op::Bis>(Register, 0, RegisterDeferred, 1);

  t.expect_word(DATA, 0000030);
  t.expect_reg(1, DATA);
  t.expect_flags(flag::none);
}

TEST(bit_word_set)
{
  t.reset(flag::c);
  t.reg(0) = 0000010;
  t.reg(1) = 0000030;

  t.execute_double<op::Bit>(Register, 0, Register, 1);

  t.expect_reg(1, 0000030); // BIT does not write
  t.expect_flags(flag::c);
}

TEST(bit_word_zero)
{
  t.reset(flag::c);
  t.reg(0) = 1;
  t.reg(1) = 2;

  t.execute_double<op::Bit>(Register, 0, Register, 1);

  t.expect_flags(flag::z | flag::c);
}

TEST(bitb_high_bit)
{
  t.reset(flag::c);
  t.reg(0) = 0x0080;
  t.reg(1) = 0x0080;

  t.execute_double<op::Bit, true>(Register, 0, Register, 1);

  t.expect_reg(1, 0x0080);
  t.expect_flags(flag::n | flag::c);
}

TEST(bic_word_register)
{
  t.reset(flag::c);
  t.reg(0) = 0000017;
  t.reg(1) = 001234;

  t.execute_double<op::Bic>(Register, 0, Register, 1);

  t.expect_reg(1, 001220);
  t.expect_flags(flag::c);
}

TEST(bicb_register_preserves_high_byte)
{
  t.reset(flag::c);
  t.reg(0) = 0x000f;
  t.reg(1) = 0x1234;

  t.execute_double<op::Bic, true>(Register, 0, Register, 1);

  t.expect_reg(1, 0x1230);
  t.expect_flags(flag::c);
}
