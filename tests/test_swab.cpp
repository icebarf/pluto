/*
 * SWAB tests.
 *
 * Handbook reference: page 57.
 *   Swap the high and low bytes.
 *   N: set if bit 7 of the low-order byte of the result is set.
 *   Z: set if the low-order byte of the result is zero.
 *   V: cleared. C: cleared.
 */
#include "test_framework.hpp"

TEST(swab_example)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0077777;

  m.execute(single_op(003, MODE_REGISTER, 0));

  // Handbook example: 077777 -> 177577.
  EXPECT_REG(0, 0177577);
  EXPECT_FLAGS(0, 0, 0, 0);
}

TEST(swab_zero)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0;

  m.execute(single_op(003, MODE_REGISTER, 0));

  EXPECT_REG(0, 0);
  EXPECT_FLAGS(0, 1, 0, 0);
}

TEST(swab_zero_low_byte_with_nonzero_high_byte)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0000012; // 0x000A -> 0x0A00 after swap

  m.execute(single_op(003, MODE_REGISTER, 0));

  EXPECT_REG(0, 005000); // 0x0A00
  // Z is defined on the low-order byte of the result, which is zero.
  EXPECT_FLAGS(0, 1, 0, 0);
}

TEST(swab_negative_from_low_byte)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 0100000; // 0x8000 -> 0x0080 after swap

  m.execute(single_op(003, MODE_REGISTER, 0));

  EXPECT_REG(0, 0000200);
  // N is defined on bit 7 of the low-order byte of the result.
  EXPECT_FLAGS(1, 0, 0, 0);
}

TEST(swab_clears_v_and_c)
{
  TestMachine m;
  m.reset(017);
  m.registers[0] = 012345;

  m.execute(single_op(003, MODE_REGISTER, 0));

  EXPECT_REG(0, 0162424); // 0x14E5 -> 0xE514
  EXPECT_FLAGS(0, 0, 0, 0);
}

TEST(swab_memory_word)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  write_word(m.memory, TestMachine::DATA, 0077777);

  m.execute(single_op(003, MODE_REGISTER_DEFERRED, 0));

  EXPECT_WORD(TestMachine::DATA, 0177577);
  EXPECT_REG(0, TestMachine::DATA);
  EXPECT_FLAGS(0, 0, 0, 0);
}
