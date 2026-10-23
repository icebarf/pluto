/*
 * NEG / NEGB tests.
 *
 * Handbook reference: page 45.
 *   N/Z from result.
 *   V set if result is 100000 word (200 byte).
 *   C cleared if result is 0; set otherwise.
 */
#include "test_framework.hpp"

TEST(neg_word_example)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0000010;

  m.execute(single_op(054, MODE_REGISTER, 0));

  // Handbook example: 000010 -> 177770, NZVC = 1001.
  EXPECT_REG(0, 0177770);
  EXPECT_FLAGS(1, 0, 0, 1);
}

TEST(neg_word_zero)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0;

  m.execute(single_op(054, MODE_REGISTER, 0));

  EXPECT_REG(0, 0);
  EXPECT_FLAGS(0, 1, 0, 0); // C must be cleared when result is zero
}

TEST(neg_word_two)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 2;

  m.execute(single_op(054, MODE_REGISTER, 0));

  EXPECT_REG(0, 0177776);
  EXPECT_FLAGS(1, 0, 0, 1); // result non-zero -> C set
}

TEST(neg_word_most_negative)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0100000;

  m.execute(single_op(054, MODE_REGISTER, 0));

  EXPECT_REG(0, 0100000);
  EXPECT_FLAGS(1, 0, 1, 1);
}

TEST(neg_byte_register_preserves_high_byte)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012001;

  m.execute(single_op(054, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012377);
  EXPECT_FLAGS(1, 0, 0, 1);
}

TEST(neg_byte_zero)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012000; // low byte is zero

  m.execute(single_op(054, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012000);
  EXPECT_FLAGS(0, 1, 0, 0);
}

TEST(neg_byte_most_negative)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012200;

  m.execute(single_op(054, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012200);
  EXPECT_FLAGS(1, 0, 1, 1);
}

TEST(neg_word_autoincrement)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  write_word(m.memory, TestMachine::DATA, 1);

  m.execute(single_op(054, MODE_AUTOINCREMENT, 0));

  EXPECT_WORD(TestMachine::DATA, 0177777);
  EXPECT_REG(0, TestMachine::DATA + 2);
  EXPECT_FLAGS(1, 0, 0, 1);
}
