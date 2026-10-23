/*
 * ADC / ADCB / SBC / SBCB tests.
 *
 * Handbook references:
 *   ADC page 52:
 *     result = (dst) + C
 *     V set if (dst) was 077777 and C was 1 (0177 for byte)
 *     C set if (dst) was 177777 and C was 1 (0377 for byte)
 *     otherwise C cleared.
 *     With C = 0 the destination is effectively unchanged.
 *   SBC page 53:
 *     result = (dst) - C
 *     V set if result is 100000 (0200 for byte)
 *     C cleared if result is 0 and C was 1; set otherwise.
 *     With C = 0 the destination is effectively unchanged.
 */
#include "test_framework.hpp"

TEST(adc_word_carry_clear_retains_value)
{
  TestMachine m;
  m.reset(); // C = 0
  m.registers[0] = 5;

  m.execute(single_op(055, MODE_REGISTER, 0));

  EXPECT_REG(0, 5);
  EXPECT_FLAGS(0, 0, 0, 0);
}

TEST(adc_word_carry_set_adds_one)
{
  TestMachine m;
  m.reset(001); // C = 1
  m.registers[0] = 5;

  m.execute(single_op(055, MODE_REGISTER, 0));

  EXPECT_REG(0, 6);
  EXPECT_FLAGS(0, 0, 0, 0);
}

TEST(adc_word_carry_out)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 0177777;

  m.execute(single_op(055, MODE_REGISTER, 0));

  // Handbook p.52:
  //   C set if (dst) was 177777 and the incoming C was 1.
  //   V set if (dst) was 077777 and the incoming C was 1 -> V is clear here.
  EXPECT_REG(0, 0);
  EXPECT_FLAGS(0, 1, 0, 1);
}

TEST(adc_word_overflow)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 0077777;

  m.execute(single_op(055, MODE_REGISTER, 0));

  // Handbook p.52:
  //   V set if (dst) was 077777 and the incoming C was 1.
  //   C set if (dst) was 177777 and the incoming C was 1 -> C is clear here.
  EXPECT_REG(0, 0100000);
  EXPECT_FLAGS(1, 0, 1, 0);
}

TEST(adc_byte_carry_clear_retains_value)
{
  TestMachine m;
  m.reset(); // C = 0
  m.registers[0] = 012005;

  m.execute(single_op(055, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012005);
  EXPECT_FLAGS(0, 0, 0, 0);
}

TEST(adc_byte_carry_set_adds_one)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 012005;

  m.execute(single_op(055, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012006);
  EXPECT_FLAGS(0, 0, 0, 0);
}

TEST(adc_byte_carry_out)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 0000377;

  m.execute(single_op(055, MODE_REGISTER, 0, true));

  // Handbook p.52, byte form:
  //   C set if (dst) was 0377 and the incoming C was 1.
  //   V set if (dst) was 0177 and the incoming C was 1 -> V is clear here.
  EXPECT_REG(0, 0);
  EXPECT_FLAGS(0, 1, 0, 1);
}

TEST(adc_byte_overflow)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 012177;

  m.execute(single_op(055, MODE_REGISTER, 0, true));

  // Handbook p.52, byte form:
  //   V set if (dst) was 0177 and the incoming C was 1.
  //   C set if (dst) was 0377 and the incoming C was 1 -> C is clear here.
  EXPECT_REG(0, 012200);
  EXPECT_FLAGS(1, 0, 1, 0);
}

TEST(sbc_word_carry_clear_retains_value)
{
  TestMachine m;
  m.reset(); // C = 0
  m.registers[0] = 5;

  m.execute(single_op(056, MODE_REGISTER, 0));

  EXPECT_REG(0, 5);
  // SBC sets C unless (result == 0 && C was 1).
  EXPECT_FLAGS(0, 0, 0, 1);
}

TEST(sbc_word_carry_set_subtracts_one)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 5;

  m.execute(single_op(056, MODE_REGISTER, 0));

  EXPECT_REG(0, 4);
  EXPECT_FLAGS(0, 0, 0, 1);
}

TEST(sbc_word_result_zero_clears_carry)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 1;

  m.execute(single_op(056, MODE_REGISTER, 0));

  EXPECT_REG(0, 0);
  EXPECT_FLAGS(0, 1, 0, 0);
}

TEST(sbc_word_borrow)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 0;

  m.execute(single_op(056, MODE_REGISTER, 0));

  EXPECT_REG(0, 0177777);
  EXPECT_FLAGS(1, 0, 0, 1);
}

TEST(sbc_word_overflow)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 0100001;

  m.execute(single_op(056, MODE_REGISTER, 0));

  EXPECT_REG(0, 0100000);
  EXPECT_FLAGS(1, 0, 1, 1);
}

TEST(sbc_byte_carry_clear_retains_value)
{
  TestMachine m;
  m.reset(); // C = 0
  m.registers[0] = 012005;

  m.execute(single_op(056, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012005);
  EXPECT_FLAGS(0, 0, 0, 1);
}

TEST(sbc_byte_carry_set_subtracts_one)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 012005;

  m.execute(single_op(056, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012004);
  EXPECT_FLAGS(0, 0, 0, 1);
}

TEST(sbc_byte_result_zero_clears_carry)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 012001;

  m.execute(single_op(056, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012000);
  EXPECT_FLAGS(0, 1, 0, 0);
}

TEST(sbc_byte_borrow)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 012000;

  m.execute(single_op(056, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012377);
  EXPECT_FLAGS(1, 0, 0, 1);
}

TEST(sbc_byte_overflow)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 012201;

  m.execute(single_op(056, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012200);
  EXPECT_FLAGS(1, 0, 1, 1);
}
