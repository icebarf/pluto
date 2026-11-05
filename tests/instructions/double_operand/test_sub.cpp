/*
 * SUB tests (word only on the 11/20).
 *
 * Handbook page 62:
 *   dst <- dst - src
 *   N/Z from result, V on signed overflow, C is the borrow (set if dst < src).
 */
#include "test_framework.hpp"

using namespace test;
using enum Instructions::Mode;

TEST(sub_word_registers)
{
  t.reset();
  t.reg(0) = 1;
  t.reg(1) = 2;

  t.execute_double<op::Sub>(Register, 0, Register, 1);

  t.expect_reg(1, 1);
  t.expect_flags(flag::none);
}

TEST(sub_word_borrow)
{
  t.reset();
  t.reg(0) = 1;
  t.reg(1) = 0;

  t.execute_double<op::Sub>(Register, 0, Register, 1);

  t.expect_reg(1, 0177777);
  t.expect_flags(flag::n | flag::c);
}

TEST(sub_word_no_borrow_negative_result)
{
  t.reset();
  t.reg(0) = 0;
  t.reg(1) = 0100000;

  t.execute_double<op::Sub>(Register, 0, Register, 1);

  t.expect_reg(1, 0100000);
  t.expect_flags(flag::n); // no borrow
}

TEST(sub_word_overflow)
{
  t.reset();
  t.reg(0) = 1;
  t.reg(1) = 0100000;

  t.execute_double<op::Sub>(Register, 0, Register, 1);

  t.expect_reg(1, 0077777);
  t.expect_flags(flag::v);
}

TEST(sub_word_negative_overflow)
{
  t.reset();
  t.reg(0) = 0177777;
  t.reg(1) = 0077777;

  t.execute_double<op::Sub>(Register, 0, Register, 1);

  t.expect_reg(1, 0100000);
  // dst 077777 < src 177777, so subtraction borrows.
  t.expect_flags(flag::n | flag::v | flag::c);
}

TEST(sub_word_memory_source)
{
  t.reset();
  t.reg(1) = DATA;
  t.reg(0) = 10;
  t.set_word(DATA, 5);

  t.execute_double<op::Sub>(RegisterDeferred, 1, Register, 0);

  t.expect_reg(0, 5);
  t.expect_flags(flag::none);
}
