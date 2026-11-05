/*
 * Addressing-mode side-effect tests.
 *
 * Handbook references:
 *   Autoincrement/autodecrement step by 1 for byte operands, 2 for word
 *   operands, and always by 2 for SP and PC (sections 3.3.2, 3.3.3, 3.6).
 *   Deferred autoincrement/autodecrement are always by 2, even for bytes
 *   (section 3.4).
 *   Index uses a word following the instruction and advances PC past it
 *   (section 3.3.4).
 */
#include "test_framework.hpp"

using namespace test;
using enum Instructions::Mode;

TEST(clrb_immediate_pc_steps_two)
{
  t.reset();

  // CLRB #0125715 (0xABCD): immediate byte destination is the word at 01002.
  // PC still advances by two for a byte operand, and only the low byte clears.
  t.execute<op::Clr, true>(AutoIncrement, 7, 0125715);

  t.expect_word(01002, 0125400); // 0xAB00
  t.expect_pc(01004);
}

TEST(clrb_sp_autodecrement_steps_two)
{
  t.reset();
  t.reg(6) = DATA + 2;
  t.set_byte(DATA, 0xab);

  t.execute<op::Clr, true>(AutoDecrement, 6);

  t.expect_reg(6, DATA);
  t.expect_byte(DATA, 0);
}

TEST(clrb_autoincrement_deferred_steps_two)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_word(DATA, TARGET);
  t.set_byte(TARGET, 0xab);

  t.execute<op::Clr, true>(AutoIncrementDeferred, 0);

  t.expect_reg(0, DATA + 2);
  t.expect_byte(TARGET, 0);
  t.expect_pc(01002);
}

TEST(clrb_autodecrement_deferred_steps_two)
{
  t.reset();
  t.reg(0) = DATA + 2;
  t.set_word(DATA, TARGET);
  t.set_byte(TARGET, 0xab);

  t.execute<op::Clr, true>(AutoDecrementDeferred, 0);

  t.expect_reg(0, DATA);
  t.expect_byte(TARGET, 0);
}

TEST(clrb_index_deferred)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_word(DATA + 4, TARGET);
  t.set_byte(TARGET, 0xab);

  t.execute<op::Clr, true>(IndexDeferred, 0, 4);

  t.expect_byte(TARGET, 0);
  t.expect_reg(0, DATA);
  t.expect_pc(01004);
}

TEST(clrb_relative_deferred)
{
  t.reset();
  t.set_word(01010, TARGET);
  t.set_byte(TARGET, 0xab);

  t.execute<op::Clr, true>(IndexDeferred, 7, 4);

  t.expect_byte(TARGET, 0);
  t.expect_pc(01004);
}

TEST(com_autoincrement_side_effects_once)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_word(DATA, 0);

  t.execute<op::Com>(AutoIncrement, 0);

  t.expect_word(DATA, 0177777);
  t.expect_reg(0, DATA + 2);
  t.expect_pc(01002);
  t.expect_flags(flag::n | flag::c);
}

TEST(inc_index_does_not_modify_base_register)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_word(DATA + 4, 5);

  t.execute<op::Inc>(Index, 0, 4);

  t.expect_word(DATA + 4, 6);
  t.expect_reg(0, DATA);
  t.expect_pc(01004);
}

TEST(clr_register_deferred_does_not_modify_register)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_word(DATA, 012345);

  // @R0 means the operand is at the address contained in R0, so this clears
  // the word at DATA. R0 itself is not modified.
  t.execute<op::Clr>(RegisterDeferred, 0);

  t.expect_word(DATA, 0);
  t.expect_reg(0, DATA);
}

TEST(clr_autodecrement_deferred)
{
  t.reset();
  t.reg(0) = DATA + 2;
  t.set_word(DATA, TARGET);
  t.set_word(TARGET, 012345);

  t.execute<op::Clr>(AutoDecrementDeferred, 0);

  t.expect_word(TARGET, 0);
  t.expect_reg(0, DATA);
}
