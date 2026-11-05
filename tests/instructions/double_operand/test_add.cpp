/*
 * ADD tests (word only on the 11/20).
 *
 * Handbook page 61:
 *   dst <- dst + src
 *   N/Z from result, V on signed overflow, C on unsigned carry out.
 */
#include "test_framework.hpp"

using namespace test;
using enum Instructions::Mode;

TEST(add_word_registers)
{
  t.reset();
  t.reg(0) = 1;
  t.reg(1) = 2;

  t.execute_double<op::Add>(Register, 0, Register, 1);

  t.expect_reg(1, 3);
  t.expect_flags(flag::none);
}

TEST(add_word_carry_and_zero)
{
  t.reset();
  t.reg(0) = 0177777;
  t.reg(1) = 1;

  t.execute_double<op::Add>(Register, 0, Register, 1);

  t.expect_reg(1, 0);
  t.expect_flags(flag::z | flag::c);
}

TEST(add_word_overflow_positive)
{
  t.reset();
  t.reg(0) = 1;
  t.reg(1) = 0077777;

  t.execute_double<op::Add>(Register, 0, Register, 1);

  t.expect_reg(1, 0100000);
  t.expect_flags(flag::n | flag::v);
}

TEST(add_word_overflow_negative)
{
  t.reset();
  t.reg(0) = 0177777;
  t.reg(1) = 0100000;

  t.execute_double<op::Add>(Register, 0, Register, 1);

  t.expect_reg(1, 0077777);
  t.expect_flags(flag::c | flag::v);
}

TEST(add_word_no_carry_negative_result)
{
  t.reset();
  t.reg(0) = 0177777;
  t.reg(1) = 0;

  t.execute_double<op::Add>(Register, 0, Register, 1);

  t.expect_reg(1, 0177777);
  t.expect_flags(flag::n); // no carry out
}

TEST(add_word_memory_source)
{
  t.reset();
  t.reg(1) = DATA;
  t.reg(0) = 10;
  t.set_word(DATA, 5);

  t.execute_double<op::Add>(RegisterDeferred, 1, Register, 0);

  t.expect_reg(0, 15);
  t.expect_flags(flag::none);
}
