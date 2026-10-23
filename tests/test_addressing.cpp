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

TEST(clrb_immediate_pc_steps_two)
{
  TestMachine m;
  m.reset();

  // CLRB #0125715 (0xABCD): immediate byte destination is the word at 01002.
  // PC still advances by two for a byte operand, and only the low byte clears.
  m.execute(single_op(050, MODE_AUTOINCREMENT, 7, true), { 0125715 });

  EXPECT_WORD(01002, 0125400); // 0xAB00
  EXPECT_PC(01004);
}

TEST(clrb_sp_autodecrement_steps_two)
{
  TestMachine m;
  m.reset();
  m.registers[6] = TestMachine::DATA + 2;
  m.memory[TestMachine::DATA] = 0xab;

  m.execute(single_op(050, MODE_AUTODECREMENT, 6, true));

  EXPECT_REG(6, TestMachine::DATA);
  EXPECT_BYTE(TestMachine::DATA, 0);
}

TEST(clrb_autoincrement_deferred_steps_two)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  write_word(m.memory, TestMachine::DATA, TestMachine::TARGET);
  m.memory[TestMachine::TARGET] = 0xab;

  m.execute(single_op(050, MODE_AUTOINCREMENT_DEFERRED, 0, true));

  EXPECT_REG(0, TestMachine::DATA + 2);
  EXPECT_BYTE(TestMachine::TARGET, 0);
  EXPECT_PC(01002);
}

TEST(clrb_autodecrement_deferred_steps_two)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA + 2;
  write_word(m.memory, TestMachine::DATA, TestMachine::TARGET);
  m.memory[TestMachine::TARGET] = 0xab;

  m.execute(single_op(050, MODE_AUTODECREMENT_DEFERRED, 0, true));

  EXPECT_REG(0, TestMachine::DATA);
  EXPECT_BYTE(TestMachine::TARGET, 0);
}

TEST(clrb_index_deferred)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  write_word(m.memory, TestMachine::DATA + 4, TestMachine::TARGET);
  m.memory[TestMachine::TARGET] = 0xab;

  m.execute(single_op(050, MODE_INDEX_DEFERRED, 0, true), { 4 });

  EXPECT_BYTE(TestMachine::TARGET, 0);
  EXPECT_REG(0, TestMachine::DATA);
  EXPECT_PC(01004);
}

TEST(clrb_relative_deferred)
{
  TestMachine m;
  m.reset();
  write_word(m.memory, 01010, TestMachine::TARGET);
  m.memory[TestMachine::TARGET] = 0xab;

  m.execute(single_op(050, MODE_INDEX_DEFERRED, 7, true), { 4 });

  EXPECT_BYTE(TestMachine::TARGET, 0);
  EXPECT_PC(01004);
}

TEST(com_autoincrement_side_effects_once)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  write_word(m.memory, TestMachine::DATA, 0);

  m.execute(single_op(051, MODE_AUTOINCREMENT, 0));

  EXPECT_WORD(TestMachine::DATA, 0177777);
  EXPECT_REG(0, TestMachine::DATA + 2);
  EXPECT_PC(01002);
}

TEST(inc_index_does_not_modify_base_register)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  write_word(m.memory, TestMachine::DATA + 4, 5);

  m.execute(single_op(052, MODE_INDEX, 0), { 4 });

  EXPECT_WORD(TestMachine::DATA + 4, 6);
  EXPECT_REG(0, TestMachine::DATA);
  EXPECT_PC(01004);
}

TEST(clr_register_deferred_does_not_modify_register)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  write_word(m.memory, TestMachine::DATA, 012345);

  // @R0 means the operand is at the address contained in R0, so this clears
  // the word at DATA. R0 itself is not modified.
  m.execute(single_op(050, MODE_REGISTER_DEFERRED, 0));

  EXPECT_WORD(TestMachine::DATA, 0);
  EXPECT_REG(0, TestMachine::DATA);
}

TEST(clr_autodecrement_deferred)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA + 2;
  write_word(m.memory, TestMachine::DATA, TestMachine::TARGET);
  write_word(m.memory, TestMachine::TARGET, 012345);

  m.execute(single_op(050, MODE_AUTODECREMENT_DEFERRED, 0));

  EXPECT_WORD(TestMachine::TARGET, 0);
  EXPECT_REG(0, TestMachine::DATA);
}
