/*
 * COM / COMB tests.
 *
 * Handbook reference: page 47.
 *   N/Z from result, V cleared, C set.
 *   Byte: only the low byte is complemented; register high byte preserved.
 */
#include "test_framework.hpp"

TEST(com_word_example)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 013333;

  m.execute(single_op(051, MODE_REGISTER, 0));

  // Handbook example: 013333 -> 164444, NZVC = 1001.
  EXPECT_REG(0, 0164444);
  EXPECT_FLAGS(1, 0, 0, 1);
}

TEST(com_word_zero)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0;

  m.execute(single_op(051, MODE_REGISTER, 0));

  EXPECT_REG(0, 0177777);
  EXPECT_FLAGS(1, 0, 0, 1);
}

TEST(com_word_all_ones)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0177777;

  m.execute(single_op(051, MODE_REGISTER, 0));

  EXPECT_REG(0, 0);
  EXPECT_FLAGS(0, 1, 0, 1);
}

TEST(com_byte_preserves_high_byte)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0022222;

  m.execute(single_op(051, MODE_REGISTER, 0, true));

  // Low byte 0222 -> 0155, high byte 0022 is preserved.
  EXPECT_REG(0, 0022155);
  EXPECT_FLAGS(0, 0, 0, 1);
}

TEST(com_byte_zero)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0000377;

  m.execute(single_op(051, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 0);
  EXPECT_FLAGS(0, 1, 0, 1);
}

TEST(com_word_memory)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  write_word(m.memory, TestMachine::DATA, 013333);

  m.execute(single_op(051, MODE_REGISTER_DEFERRED, 0));

  EXPECT_WORD(TestMachine::DATA, 0164444);
  EXPECT_REG(0, TestMachine::DATA);
  EXPECT_FLAGS(1, 0, 0, 1);
}

TEST(com_byte_memory_does_not_touch_neighbor)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  m.memory[TestMachine::DATA] = 0xab;
  m.memory[TestMachine::DATA + 1] = 0xcd;

  m.execute(single_op(051, MODE_REGISTER_DEFERRED, 0, true));

  EXPECT_BYTE(TestMachine::DATA, 0x54);
  EXPECT_BYTE(TestMachine::DATA + 1, 0xcd);
  EXPECT_FLAGS(0, 0, 0, 1);
}
