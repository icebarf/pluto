/*
 * TST / TSTB tests.
 *
 * Handbook reference: page 46.
 *   N/Z from the destination, V and C cleared.
 *   The destination is not modified.
 */
#include "test_framework.hpp"

TEST(tst_word_positive)
{
  TestMachine m;
  m.reset(017);
  m.registers[0] = 0000333;

  m.execute(single_op(057, MODE_REGISTER, 0));

  EXPECT_REG(0, 0000333);
  EXPECT_FLAGS(0, 0, 0, 0);
}

TEST(tst_word_zero)
{
  TestMachine m;
  m.reset(017);
  m.registers[0] = 0;

  m.execute(single_op(057, MODE_REGISTER, 0));

  EXPECT_REG(0, 0);
  EXPECT_FLAGS(0, 1, 0, 0);
}

TEST(tst_word_negative)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0100000;

  m.execute(single_op(057, MODE_REGISTER, 0));

  EXPECT_REG(0, 0100000);
  EXPECT_FLAGS(1, 0, 0, 0);
}

TEST(tst_byte_negative)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0000200;

  m.execute(single_op(057, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 0000200);
  EXPECT_FLAGS(1, 0, 0, 0);
}

TEST(tst_byte_zero_ignores_high_byte)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012000; // low byte is zero, high byte is not

  m.execute(single_op(057, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012000);
  EXPECT_FLAGS(0, 1, 0, 0);
}

TEST(tst_memory_is_not_modified)
{
  TestMachine m;
  m.reset(017);
  m.registers[0] = TestMachine::DATA;
  write_word(m.memory, TestMachine::DATA, 012345);

  m.execute(single_op(057, MODE_REGISTER_DEFERRED, 0));

  EXPECT_WORD(TestMachine::DATA, 012345);
  EXPECT_REG(0, TestMachine::DATA);
  EXPECT_FLAGS(0, 0, 0, 0);
}
