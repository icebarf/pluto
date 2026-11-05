/*
 * CMP tests (word form; CMPB is not implemented in the current source).
 *
 * Handbook page 63:
 *   flags from (src) - (dst), operands are not modified.
 *   N/Z from result, V on signed overflow, C is the borrow (src < dst).
 */
#include "test_framework.hpp"

using namespace test;
using enum Instructions::Mode;

TEST(cmp_word_equal)
{
  t.reset();
  t.reg(0) = 5;
  t.reg(1) = 5;

  t.execute_double<op::Cmp>(Register, 0, Register, 1);

  t.expect_reg(0, 5);
  t.expect_reg(1, 5);
  t.expect_flags(flag::z);
}

TEST(cmp_word_source_less)
{
  t.reset();
  t.reg(0) = 1;
  t.reg(1) = 2;

  t.execute_double<op::Cmp>(Register, 0, Register, 1);

  t.expect_flags(flag::n | flag::c);
}

TEST(cmp_word_source_greater)
{
  t.reset();
  t.reg(0) = 2;
  t.reg(1) = 1;

  t.execute_double<op::Cmp>(Register, 0, Register, 1);

  t.expect_flags(flag::none);
}

TEST(cmp_word_overflow)
{
  t.reset();
  t.reg(0) = 0100000;
  t.reg(1) = 1;

  t.execute_double<op::Cmp>(Register, 0, Register, 1);

  t.expect_flags(flag::v);
}

TEST(cmp_word_no_borrow_negative_result)
{
  t.reset();
  t.reg(0) = 0100000;
  t.reg(1) = 0;

  t.execute_double<op::Cmp>(Register, 0, Register, 1);

  t.expect_flags(flag::n); // src >= dst, so no borrow
}

TEST(cmp_word_memory_source)
{
  t.reset();
  t.reg(1) = DATA;
  t.reg(0) = 10;
  t.set_word(DATA, 5);

  t.execute_double<op::Cmp>(RegisterDeferred, 1, Register, 0);

  t.expect_reg(0, 10);
  t.expect_reg(1, DATA);
  t.expect_flags(flag::n | flag::c);
}
