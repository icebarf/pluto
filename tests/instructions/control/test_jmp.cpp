/*
 * JMP tests.
 *
 * Handbook page 106:
 *   PC <- destination effective address.
 *   Register mode (0) is illegal; all other destination modes are allowed.
 *   In destination mode 2 (autoincrement), JMP increments the register before
 *   using it as the destination address.
 */
#include "test_framework.hpp"

using namespace test;
using enum Instructions::Mode;

TEST(jmp_register_deferred)
{
  t.reset(flag::all);
  t.reg(1) = TARGET;

  t.execute_jmp(RegisterDeferred, 1);

  t.expect_pc(TARGET);
  t.expect_flags(flag::all);
}

TEST(jmp_index)
{
  t.reset();
  t.reg(1) = DATA;

  t.execute_jmp(Index, 1, 4);

  t.expect_pc(DATA + 4);
  t.expect_reg(1, DATA);
}

TEST(jmp_index_deferred)
{
  t.reset();
  t.reg(1) = DATA;
  t.set_word(DATA + 4, TARGET);

  t.execute_jmp(IndexDeferred, 1, 4);

  t.expect_pc(TARGET);
  t.expect_reg(1, DATA);
}

TEST(jmp_absolute)
{
  t.reset();

  t.execute_jmp(AutoIncrementDeferred, 7, TARGET);

  t.expect_pc(TARGET);
}

TEST(jmp_relative)
{
  t.reset();

  t.execute_jmp(Index, 7, 4);

  // PC after the index word is 01004; offset 4 selects 01010.
  t.expect_pc(01010);
}

TEST(jmp_autoincrement_increments_before_use)
{
  t.reset();
  t.reg(1) = DATA;

  t.execute_jmp(AutoIncrement, 1);

  t.expect_reg(1, DATA + 2);
  t.expect_pc(DATA + 2);
}
