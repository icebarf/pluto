/*
 * CLR / CLRB tests.
 *
 * Handbook reference: page 42.
 *   N: cleared, Z: set, V: cleared, C: cleared.
 *   Word: destination = 0. Byte: low byte = 0, register high byte preserved.
 */
#include "test_framework.hpp"

using namespace test;
using enum Instructions::Mode;

TEST(clr_word_register)
{
  t.reset(flag::all); // all flags set, to prove they are cleared
  t.reg(0) = 012345;

  t.execute<op::Clr>(Register, 0);

  t.expect_reg(0, 0);
  t.expect_flags(flag::z);
  t.expect_pc(01002);
}

TEST(clr_byte_register_preserves_high_byte)
{
  t.reset();
  t.reg(0) = 012345;

  t.execute<op::Clr, true>(Register, 0);

  // 012345 = 0x14E5; only the low byte (0xE5) is cleared.
  t.expect_reg(0, 012000); // 0x1400
  t.expect_flags(flag::z);
}

TEST(clr_word_autoincrement)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_word(DATA, 012345);

  t.execute<op::Clr>(AutoIncrement, 0);

  t.expect_word(DATA, 0);
  t.expect_reg(0, DATA + 2);
  t.expect_pc(01002);
}

TEST(clr_byte_autoincrement_steps_one)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_byte(DATA, 0xab);
  t.set_byte(DATA + 1, 0xcd);

  t.execute<op::Clr, true>(AutoIncrement, 0);

  t.expect_byte(DATA, 0);
  t.expect_byte(DATA + 1, 0xcd);
  t.expect_reg(0, DATA + 1);
}

TEST(clr_word_register_deferred)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_word(DATA, 012345);

  // @R0: R0 contains the address of the operand.
  t.execute<op::Clr>(RegisterDeferred, 0);

  t.expect_word(DATA, 0);
  t.expect_reg(0, DATA);
}

TEST(clr_word_autodecrement)
{
  t.reset();
  t.reg(0) = DATA + 2;
  t.set_word(DATA, 012345);

  t.execute<op::Clr>(AutoDecrement, 0);

  t.expect_word(DATA, 0);
  t.expect_reg(0, DATA);
}

TEST(clr_word_index)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_word(DATA + 4, 012345);

  t.execute<op::Clr>(Index, 0, 4);

  t.expect_word(DATA + 4, 0);
  t.expect_reg(0, DATA);
  t.expect_pc(01004);
}

TEST(clr_word_absolute)
{
  t.reset();
  t.set_word(TARGET, 012345);

  t.execute<op::Clr>(AutoIncrementDeferred, 7, TARGET);

  t.expect_word(TARGET, 0);
  t.expect_pc(01004);
}

TEST(clr_word_relative)
{
  t.reset();
  // PC after the offset word is 01004; offset 4 selects 01010.
  t.set_word(01010, 012345);

  t.execute<op::Clr>(Index, 7, 4);

  t.expect_word(01010, 0);
  t.expect_pc(01004);
}

TEST(clr_word_relative_deferred)
{
  t.reset();
  t.set_word(01010, TARGET);
  t.set_word(TARGET, 012345);

  t.execute<op::Clr>(IndexDeferred, 7, 4);

  t.expect_word(TARGET, 0);
  t.expect_pc(01004);
}

TEST(clr_word_immediate_clears_operand_word)
{
  t.reset();

  t.execute<op::Clr>(AutoIncrement, 7, 012345);

  t.expect_word(01002, 0);
  t.expect_pc(01004);
}

TEST(clr_register_does_not_write_memory)
{
  t.reset();
  t.reg(0) = 012345;
  t.set_word(0, 012345); // sentinel in the trap vector area

  t.execute<op::Clr>(Register, 0);

  t.expect_word(0, 012345);
}
