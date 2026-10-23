/*
 * ASL / ASR / ROL / ROR tests (word and byte forms).
 *
 * Handbook references:
 *   ASR page 49: C <- old bit 0; N/Z from result; V = N xor C.
 *   ASL page 50: C <- old bit 15 (bit 7 for byte); N/Z; V = N xor C.
 *   ROL page 55: C <- old bit 15 (bit 7); old C -> bit 0; V = N xor C.
 *   ROR page 56: C <- old bit 0; old C -> bit 15 (bit 7); V = N xor C.
 *
 * For byte variants, N is result bit 7 and Z is the low byte being zero.
 * Register high bytes are preserved.
 */
#include "test_framework.hpp"

TEST(asl_word_one)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 1;

  m.execute(single_op(063, MODE_REGISTER, 0));

  EXPECT_REG(0, 2);
  EXPECT_FLAGS(0, 0, 0, 0);
}

TEST(asl_word_msb_out)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0100000;

  m.execute(single_op(063, MODE_REGISTER, 0));

  EXPECT_REG(0, 0);
  EXPECT_FLAGS(0, 1, 1, 1);
}

TEST(asl_word_overflow)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0040000;

  m.execute(single_op(063, MODE_REGISTER, 0));

  EXPECT_REG(0, 0100000);
  EXPECT_FLAGS(1, 0, 1, 0);
}

TEST(asl_word_carry_and_result)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0100001;

  m.execute(single_op(063, MODE_REGISTER, 0));

  EXPECT_REG(0, 2);
  EXPECT_FLAGS(0, 0, 1, 1);
}

TEST(aslb_msb_out_preserves_high_byte)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012200;

  m.execute(single_op(063, MODE_REGISTER, 0, true));

  // 012200 = 0x1480; low byte 0x80 shifts out, high byte 0x14 survives.
  EXPECT_REG(0, 012000); // 0x1400
  EXPECT_FLAGS(0, 1, 1, 1);
}

TEST(aslb_overflow)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012100;

  m.execute(single_op(063, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012200);
  EXPECT_FLAGS(1, 0, 1, 0);
}

TEST(asr_word_four)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 4;

  m.execute(single_op(062, MODE_REGISTER, 0));

  EXPECT_REG(0, 2);
  EXPECT_FLAGS(0, 0, 0, 0);
}

TEST(asr_word_sign_replicated)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0100000;

  m.execute(single_op(062, MODE_REGISTER, 0));

  EXPECT_REG(0, 0140000);
  EXPECT_FLAGS(1, 0, 1, 0);
}

TEST(asr_word_lsb_out)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 1;

  m.execute(single_op(062, MODE_REGISTER, 0));

  EXPECT_REG(0, 0);
  EXPECT_FLAGS(0, 1, 1, 1);
}

TEST(asr_word_odd_negative)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0100001;

  m.execute(single_op(062, MODE_REGISTER, 0));

  EXPECT_REG(0, 0140000);
  EXPECT_FLAGS(1, 0, 0, 1);
}

TEST(asrb_sign_replicated_preserves_high_byte)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012200;

  m.execute(single_op(062, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012300);
  EXPECT_FLAGS(1, 0, 1, 0);
}

TEST(asrb_lsb_out)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012001;

  m.execute(single_op(062, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012000);
  EXPECT_FLAGS(0, 1, 1, 1);
}

TEST(asrb_odd_negative)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012201;

  m.execute(single_op(062, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012300);
  EXPECT_FLAGS(1, 0, 0, 1);
}

TEST(rol_word_one)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 1;

  m.execute(single_op(061, MODE_REGISTER, 0));

  EXPECT_REG(0, 2);
  EXPECT_FLAGS(0, 0, 0, 0);
}

TEST(rol_word_carry_in)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 1;

  m.execute(single_op(061, MODE_REGISTER, 0));

  EXPECT_REG(0, 3);
  EXPECT_FLAGS(0, 0, 0, 0);
}

TEST(rol_word_msb_out)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0100000;

  m.execute(single_op(061, MODE_REGISTER, 0));

  EXPECT_REG(0, 0);
  EXPECT_FLAGS(0, 1, 1, 1);
}

TEST(rol_word_msb_out_with_carry_in)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 0100001;

  m.execute(single_op(061, MODE_REGISTER, 0));

  EXPECT_REG(0, 3);
  EXPECT_FLAGS(0, 0, 1, 1);
}

TEST(rolb_msb_out_preserves_high_byte)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012200;

  m.execute(single_op(061, MODE_REGISTER, 0, true));

  // 012200 = 0x1480; low byte 0x80 rotates out, high byte 0x14 survives.
  EXPECT_REG(0, 012000); // 0x1400
  EXPECT_FLAGS(0, 1, 1, 1);
}

TEST(rolb_carry_in)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 012001;

  m.execute(single_op(061, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012003);
  EXPECT_FLAGS(0, 0, 0, 0);
}

TEST(ror_word_two)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 2;

  m.execute(single_op(060, MODE_REGISTER, 0));

  EXPECT_REG(0, 1);
  EXPECT_FLAGS(0, 0, 0, 0);
}

TEST(ror_word_carry_in)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 2;

  m.execute(single_op(060, MODE_REGISTER, 0));

  EXPECT_REG(0, 0100001);
  EXPECT_FLAGS(1, 0, 1, 0);
}

TEST(ror_word_lsb_out)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 1;

  m.execute(single_op(060, MODE_REGISTER, 0));

  EXPECT_REG(0, 0);
  EXPECT_FLAGS(0, 1, 1, 1);
}

TEST(ror_word_lsb_out_with_carry_in)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 1;

  m.execute(single_op(060, MODE_REGISTER, 0));

  EXPECT_REG(0, 0100000);
  EXPECT_FLAGS(1, 0, 0, 1);
}

TEST(rorb_lsb_out_preserves_high_byte)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012001;

  m.execute(single_op(060, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012000);
  EXPECT_FLAGS(0, 1, 1, 1);
}

TEST(rorb_carry_in)
{
  TestMachine m;
  m.reset(001);
  m.registers[0] = 012001;

  m.execute(single_op(060, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012200);
  EXPECT_FLAGS(1, 0, 0, 1);
}
