/*
 * Operand-resolution unit tests.
 *
 * resolve_operand() must apply addressing side effects exactly once.
 * read_operand() and write_operand() must be pure with respect to those side
 * effects. This is the property that makes read-modify-write instructions work.
 */
#include "test_framework.hpp"

TEST(register_word_write_does_not_touch_memory)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012345;
  write_word(m.memory, 0, 012345); // sentinel at the instruction/trap area

  m.execute(single_op(050, MODE_REGISTER, 0));

  EXPECT_REG(0, 0);
  EXPECT_WORD(0, 012345);
}

TEST(register_byte_write_does_not_touch_memory)
{
  TestMachine m;
  m.reset();
  m.registers[0] = 012345;
  m.memory[0] = 0xaa;
  m.memory[1] = 0xbb;

  m.execute(single_op(050, MODE_REGISTER, 0, true));

  EXPECT_REG(0, 012000); // 012345 = 0x14E5; high byte 0x14 survives
  EXPECT_BYTE(0, 0xaa);
  EXPECT_BYTE(1, 0xbb);
}

TEST(resolve_autoincrement_applies_side_effect_once)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;
  write_word(m.memory, TestMachine::DATA, 5);

  const Instructions::Operand operand = Instructions::resolve_operand(
    m.memory, m.registers, 0, Instructions::Mode::AutoIncrement, false);

  EXPECT_REG(0, TestMachine::DATA + 2);
  EXPECT_EQ(operand.is_register, false);
  EXPECT_EQ(operand.address, TestMachine::DATA);

  EXPECT_EQ(
    Instructions::read_operand(m.memory, m.registers, operand, false), 5);
  EXPECT_REG(0, TestMachine::DATA + 2);

  Instructions::write_operand(m.memory, m.registers, operand, 7, false);
  EXPECT_WORD(TestMachine::DATA, 7);
  EXPECT_REG(0, TestMachine::DATA + 2);
}

TEST(resolve_byte_autoincrement_steps_one)
{
  TestMachine m;
  m.reset();
  m.registers[0] = TestMachine::DATA;

  const Instructions::Operand operand = Instructions::resolve_operand(
    m.memory, m.registers, 0, Instructions::Mode::AutoIncrement, true);

  EXPECT_REG(0, TestMachine::DATA + 1);
  EXPECT_EQ(operand.address, TestMachine::DATA);
}

TEST(resolve_sp_byte_autoincrement_steps_two)
{
  TestMachine m;
  m.reset();
  m.registers[6] = TestMachine::DATA;

  const Instructions::Operand operand = Instructions::resolve_operand(
    m.memory, m.registers, 6, Instructions::Mode::AutoIncrement, true);

  EXPECT_REG(6, TestMachine::DATA + 2);
  EXPECT_EQ(operand.address, TestMachine::DATA);
}

TEST(resolve_index_uses_pc_after_offset)
{
  TestMachine m;
  m.reset();
  m.registers[Types::program_counter] = TestMachine::CODE + 2;
  write_word(m.memory, TestMachine::CODE + 2, 4);

  const Instructions::Operand operand = Instructions::resolve_operand(
    m.memory, m.registers, 7, Instructions::Mode::Index, false);

  // PC advances past the index word, and the address uses the new PC value.
  EXPECT_REG(7, TestMachine::CODE + 4);
  EXPECT_EQ(operand.address, static_cast<uint16_t>(TestMachine::CODE + 4 + 4));
}
