/*
 * INC / INCB / DEC / DECB tests.
 *
 * Handbook references:
 *   INC  page 44: N/Z result, V if (dst) held 077777 (0177 byte), C unaffected.
 *   DEC  page 43: N/Z result, V if (dst) was 100000 (0200 byte), C unaffected.
 */
#include "test_framework.hpp"

TEST(inc_word_register)
{
  TestMachine m;
  m.reset(001); // C set: INC must leave C alone
  m.registers[0] = 0000333;

  m.execute(single_op(052, MODE_REGISTER, 0));

  EXPECT_REG(0, 0000334);
  EXPECT_FLAGS(0, 0, 0, 1);
}

TEST(inc_word_overflow)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0077777;

  m.execute(single_op(052, MODE_REGISTER, 0));

  EXPECT_REG(0, 0100000);
  EXPECT_FLAGS(1, 0, 1, 0);
}

TEST(inc_word_wrap)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0177777;

  m.execute(single_op(052, MODE_REGISTER, 0));

  EXPECT_REG(0, 0);
  EXPECT_FLAGS(0, 1, 0, 0);
}

TEST(inc_byte_register_preserves_high_byte)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 012377;

  m.execute(single_op(052, MODE_REGISTER, 0, true));

  // 012377 = 0x14FF; low byte 0xFF increments to 0x00.
  EXPECT_REG(0, 012000); // 0x1400
  EXPECT_FLAGS(0, 1, 0, 1);
}

TEST(inc_byte_overflow)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012177;

  m.execute(single_op(052, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012200);
  EXPECT_FLAGS(1, 0, 1, 0);
}

TEST(inc_word_autoincrement)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  write_word(m.memory, TestMachine::DATA, 5);

  m.execute(single_op(052, MODE_AUTOINCREMENT, 0));

  EXPECT_WORD(TestMachine::DATA, 6);
  EXPECT_REG(0, TestMachine::DATA + 2);
  EXPECT_PC(01002);
}

TEST(inc_byte_autoincrement_steps_one)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  m.memory[TestMachine::DATA] = 0xff;

  m.execute(single_op(052, MODE_AUTOINCREMENT, 0, true));

  EXPECT_BYTE(TestMachine::DATA, 0);
  EXPECT_REG(0, TestMachine::DATA + 1);
  EXPECT_FLAGS(0, 1, 0, 0);
}

TEST(inc_word_autodecrement)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA + 2;
  write_word(m.memory, TestMachine::DATA, 5);

  m.execute(single_op(052, MODE_AUTODECREMENT, 0));

  EXPECT_WORD(TestMachine::DATA, 6);
  EXPECT_REG(0, TestMachine::DATA);
}

TEST(inc_word_index)
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

TEST(dec_word_register)
{
  TestMachine m;
  m.reset(001); // C set: DEC must leave C alone
  m.registers[0] = 1;

  m.execute(single_op(053, MODE_REGISTER, 0));

  EXPECT_REG(0, 0);
  EXPECT_FLAGS(0, 1, 0, 1);
}

TEST(dec_word_overflow)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0100000;

  m.execute(single_op(053, MODE_REGISTER, 0));

  EXPECT_REG(0, 0077777);
  EXPECT_FLAGS(0, 0, 1, 0);
}

TEST(dec_word_wrap)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0;

  m.execute(single_op(053, MODE_REGISTER, 0));

  EXPECT_REG(0, 0177777);
  EXPECT_FLAGS(1, 0, 0, 0);
}

TEST(dec_byte_register_preserves_high_byte)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 012300;

  m.execute(single_op(053, MODE_REGISTER, 0, true));

  // 012300 = 0x14C0; low byte 0xC0 decrements to 0xBF.
  EXPECT_REG(0, 012277); // 0x14BF
  EXPECT_FLAGS(1, 0, 0, 1);
}

TEST(dec_byte_overflow)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012200;

  m.execute(single_op(053, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012177);
  EXPECT_FLAGS(0, 0, 1, 0);
}

TEST(dec_word_autoincrement)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  write_word(m.memory, TestMachine::DATA, 5);

  m.execute(single_op(053, MODE_AUTOINCREMENT, 0));

  EXPECT_WORD(TestMachine::DATA, 4);
  EXPECT_REG(0, TestMachine::DATA + 2);
}

TEST(dec_byte_autodecrement_steps_one)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA + 1;
  m.memory[TestMachine::DATA] = 1;

  m.execute(single_op(053, MODE_AUTODECREMENT, 0, true));

  EXPECT_BYTE(TestMachine::DATA, 0);
  EXPECT_REG(0, TestMachine::DATA);
  EXPECT_FLAGS(0, 1, 0, 0);
}
