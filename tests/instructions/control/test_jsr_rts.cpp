/*
 * JSR / RTS tests.
 *
 * Handbook pages 90-92:
 *   JSR: temp <- dst; SP -= 2; (SP) <- old linkage reg;
 *        linkage reg <- PC (after the JSR and its destination words);
 *        PC <- temp.
 *   RTS: PC <- linkage reg; linkage reg <- (SP)+; SP += 2.
 */
#include "test_framework.hpp"

using namespace test;
using enum Instructions::Mode;

TEST(jsr_register_deferred)
{
  t.reset(flag::all);
  t.reg(5) = 012345; // old linkage value
  t.reg(1) = TARGET;
  t.reg(6) = DATA;

  t.execute_jsr(5, RegisterDeferred, 1);

  t.expect_reg(6, DATA - 2);
  t.expect_word(DATA - 2, 012345);
  t.expect_reg(5, 01002);
  t.expect_pc(TARGET);
  t.expect_flags(flag::all);
}

TEST(jsr_absolute)
{
  t.reset();
  t.reg(5) = 012345;
  t.reg(6) = DATA;

  t.execute_jsr(5, AutoIncrementDeferred, 7, TARGET);

  t.expect_reg(6, DATA - 2);
  t.expect_word(DATA - 2, 012345);
  t.expect_reg(5, 01004); // return address follows the address word
  t.expect_pc(TARGET);
}

TEST(jsr_autoincrement_increments_before_use)
{
  t.reset();
  t.reg(5) = 012345;
  t.reg(1) = DATA;
  t.reg(6) = DATA + 4;

  t.execute_jsr(5, AutoIncrement, 1);

  t.expect_reg(1, DATA + 2);
  t.expect_reg(6, DATA + 2);
  t.expect_word(DATA + 2, 012345);
  t.expect_reg(5, 01002);
  t.expect_pc(DATA + 2);
}

TEST(rts_register)
{
  t.reset(flag::all);
  t.reg(5) = TARGET;
  t.reg(6) = DATA;
  t.set_word(DATA, 054321);

  t.execute_rts(5);

  t.expect_pc(TARGET);
  t.expect_reg(5, 054321);
  t.expect_reg(6, DATA + 2);
  t.expect_flags(flag::all);
}

TEST(rts_pc)
{
  t.reset(flag::all);
  t.reg(6) = DATA;
  t.set_word(DATA, TARGET);

  t.execute_rts(7);

  t.expect_pc(TARGET);
  t.expect_reg(6, DATA + 2);
  t.expect_flags(flag::all);
}
