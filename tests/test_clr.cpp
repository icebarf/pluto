/*
 * CLR / CLRB tests.
 *
 * Handbook reference: page 42.
 *   N: cleared, Z: set, V: cleared, C: cleared.
 *   Word: destination = 0. Byte: low byte = 0, register high byte preserved.
 */
#include "test_framework.hpp"

TEST(clr_word_register)
{
  TestMachine m;
  m.reset(017); // all flags set, to prove they are cleared
  m.registers[0] = 012345;

  m.execute(single_op(050, MODE_REGISTER, 0));

  EXPECT_REG(0, 0);
  EXPECT_FLAGS(0, 1, 0, 0);
  EXPECT_PC(01002);
}

TEST(clr_byte_register_preserves_high_byte)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012345;

  m.execute(single_op(050, MODE_REGISTER, 0, true));

  // 012345 = 0x14E5; only the low byte (0xE5) is cleared.
  EXPECT_REG(0, 012000); // 0x1400
  EXPECT_FLAGS(0, 1, 0, 0);
}

TEST(clr_word_autoincrement)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  write_word(m.memory, TestMachine::DATA, 012345);

  m.execute(single_op(050, MODE_AUTOINCREMENT, 0));

  EXPECT_WORD(TestMachine::DATA, 0);
  EXPECT_REG(0, TestMachine::DATA + 2);
  EXPECT_PC(01002);
}

TEST(clr_byte_autoincrement_steps_one)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  m.memory[TestMachine::DATA] = 0xab;
  m.memory[TestMachine::DATA + 1] = 0xcd;

  m.execute(single_op(050, MODE_AUTOINCREMENT, 0, true));

  EXPECT_BYTE(TestMachine::DATA, 0);
  EXPECT_BYTE(TestMachine::DATA + 1, 0xcd);
  EXPECT_REG(0, TestMachine::DATA + 1);
}

TEST(clr_word_register_deferred)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  write_word(m.memory, TestMachine::DATA, 012345);

  // @R0: R0 contains the address of the operand.
  m.execute(single_op(050, MODE_REGISTER_DEFERRED, 0));

  EXPECT_WORD(TestMachine::DATA, 0);
  EXPECT_REG(0, TestMachine::DATA);
}

TEST(clr_word_autodecrement)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA + 2;
  write_word(m.memory, TestMachine::DATA, 012345);

  m.execute(single_op(050, MODE_AUTODECREMENT, 0));

  EXPECT_WORD(TestMachine::DATA, 0);
  EXPECT_REG(0, TestMachine::DATA);
}

TEST(clr_word_index)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  write_word(m.memory, TestMachine::DATA + 4, 012345);

  m.execute(single_op(050, MODE_INDEX, 0), { 4 });

  EXPECT_WORD(TestMachine::DATA + 4, 0);
  EXPECT_REG(0, TestMachine::DATA);
  EXPECT_PC(01004);
}

TEST(clr_word_absolute)
{
  TestMachine m;
  m.reset();
  write_word(m.memory, TestMachine::TARGET, 012345);

  m.execute(single_op(050, MODE_AUTOINCREMENT_DEFERRED, 7),
            { TestMachine::TARGET });

  EXPECT_WORD(TestMachine::TARGET, 0);
  EXPECT_PC(01004);
}

TEST(clr_word_relative)
{
  TestMachine m;
  m.reset();
  // PC after the offset word is 01004; offset 4 selects 01010.
  write_word(m.memory, 01010, 012345);

  m.execute(single_op(050, MODE_INDEX, 7), { 4 });

  EXPECT_WORD(01010, 0);
  EXPECT_PC(01004);
}

TEST(clr_word_relative_deferred)
{
  TestMachine m;
  m.reset();
  write_word(m.memory, 01010, TestMachine::TARGET);
  write_word(m.memory, TestMachine::TARGET, 012345);

  m.execute(single_op(050, MODE_INDEX_DEFERRED, 7), { 4 });

  EXPECT_WORD(TestMachine::TARGET, 0);
  EXPECT_PC(01004);
}

TEST(clr_word_immediate_clears_operand_word)
{
  TestMachine m;
  m.reset();

  m.execute(single_op(050, MODE_AUTOINCREMENT, 7), { 012345 });

  EXPECT_WORD(01002, 0);
  EXPECT_PC(01004);
}

TEST(clr_register_does_not_write_memory)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012345;
  write_word(m.memory, 0, 012345); // sentinel in the trap vector area

  m.execute(single_op(050, MODE_REGISTER, 0));

  EXPECT_WORD(0, 012345);
}
