/*
 * Operand-resolution unit tests.
 *
 * resolve_operand() must apply addressing side effects exactly once.
 * read_operand() and write_operand() must be pure with respect to those side
 * effects. This is the property that makes read-modify-write instructions work.
 */
#include "test_framework.hpp"

using namespace test;
using enum Instructions::Mode;

TEST(register_word_write_does_not_touch_memory)
{
  t.reset();
  t.reg(0) = 012345;
  t.set_word(0, 012345); // sentinel at the instruction/trap area

  t.execute<op::Clr>(Register, 0);

  t.expect_reg(0, 0);
  t.expect_word(0, 012345);
}

TEST(register_byte_write_does_not_touch_memory)
{
  t.reset();
  t.reg(0) = 012345;
  t.set_byte(0, 0xaa);
  t.set_byte(1, 0xbb);

  t.execute<op::Clr, true>(Register, 0);

  t.expect_reg(0, 012000); // 012345 = 0x14E5; high byte 0x14 survives
  t.expect_byte(0, 0xaa);
  t.expect_byte(1, 0xbb);
}

TEST(resolve_autoincrement_applies_side_effect_once)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_word(DATA, 5);

  const auto operand = Instructions::resolve_operand(
    t.machine.memory, t.machine.registers, 0, AutoIncrement, false);

  t.expect_reg(0, DATA + 2);
  t.expect_true(!operand.is_register, "operand is memory");
  t.expect_eq(operand.address, DATA, "address");

  t.expect_eq(Instructions::read_operand(t.machine.memory,
                                         t.machine.registers,
                                         operand,
                                         false),
              5,
              "read value");
  t.expect_reg(0, DATA + 2);

  Instructions::write_operand(
    t.machine.memory, t.machine.registers, operand, 7, false);
  t.expect_word(DATA, 7);
  t.expect_reg(0, DATA + 2);
}

TEST(resolve_byte_autoincrement_steps_one)
{
  t.reset();
  t.reg(0) = DATA;

  const auto operand = Instructions::resolve_operand(
    t.machine.memory, t.machine.registers, 0, AutoIncrement, true);

  t.expect_reg(0, DATA + 1);
  t.expect_eq(operand.address, DATA, "address");
}

TEST(resolve_sp_byte_autoincrement_steps_two)
{
  t.reset();
  t.reg(6) = DATA;

  const auto operand = Instructions::resolve_operand(
    t.machine.memory, t.machine.registers, 6, AutoIncrement, true);

  t.expect_reg(6, DATA + 2);
  t.expect_eq(operand.address, DATA, "address");
}

TEST(resolve_index_uses_pc_after_offset)
{
  t.reset();
  t.reg(Types::program_counter) = CODE + 2;
  t.set_word(CODE + 2, 4);

  const auto operand = Instructions::resolve_operand(
    t.machine.memory, t.machine.registers, 7, Index, false);

  // PC advances past the index word, and the address uses the new PC value.
  t.expect_reg(7, CODE + 4);
  t.expect_eq(operand.address, CODE + 4 + 4, "address");
}
