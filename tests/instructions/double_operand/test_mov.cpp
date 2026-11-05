/*
 * MOV / MOVB tests.
 *
 * Handbook pages 59-60:
 *   MOV  dst <- src, N/Z from src, V cleared, C unaffected.
 *   MOVB to a register sign-extends the low byte; to memory it writes one byte.
 */
#include "test_framework.hpp"

using namespace test;
using enum Instructions::Mode;

TEST(mov_word_register_to_register)
{
  t.reset(flag::c);
  t.reg(0) = 012345;

  t.execute_double<op::Mov>(Register, 0, Register, 1);

  t.expect_reg(1, 012345);
  t.expect_flags(flag::c); // V clear, C unchanged
  t.expect_pc(01002);
}

TEST(mov_word_immediate)
{
  t.reset();

  t.execute_double<op::Mov>(AutoIncrement, 7, Register, 0, 012345);

  t.expect_reg(0, 012345);
  t.expect_flags(flag::none);
  t.expect_pc(01004);
}

TEST(movb_immediate_sign_extends)
{
  t.reset();

  t.execute_double<op::Mov, true>(
    AutoIncrement, 7, Register, 0, 0200);

  t.expect_reg(0, 0177600); // 0xFF80
  t.expect_flags(flag::n);
  t.expect_pc(01004);
}

TEST(movb_register_sign_extends)
{
  t.reset(flag::c);
  t.reg(0) = 0x1280;

  t.execute_double<op::Mov, true>(Register, 0, Register, 1);

  t.expect_reg(1, 0177600);
  t.expect_reg(0, 0x1280);
  t.expect_flags(flag::n | flag::c);
}

TEST(movb_to_memory_writes_one_byte)
{
  t.reset();
  t.reg(0) = 01234;
  t.reg(1) = DATA;
  t.set_byte(DATA + 1, 0xcd);

  t.execute_double<op::Mov, true>(Register, 0, RegisterDeferred, 1);

  t.expect_byte(DATA, 0234);
  t.expect_byte(DATA + 1, 0xcd);
}

TEST(mov_word_both_autoincrement_same_register)
{
  t.reset();
  t.reg(0) = DATA;
  t.set_word(DATA, 01111);
  t.set_word(DATA + 2, 02222);

  t.execute_double<op::Mov>(AutoIncrement, 0, AutoIncrement, 0);

  t.expect_word(DATA + 2, 01111);
  t.expect_reg(0, DATA + 4);
}

TEST(mov_word_memory_to_memory_absolute)
{
  t.reset();
  t.set_word(02000, 012345);

  t.execute_double<op::Mov>(AutoIncrementDeferred,
                            7,
                            AutoIncrementDeferred,
                            7,
                            02000,
                            03000);

  t.expect_word(03000, 012345);
  t.expect_pc(01006);
}

TEST(mov_word_index_deferred_source)
{
  t.reset();
  t.reg(1) = DATA;
  t.set_word(DATA + 4, 02000);
  t.set_word(02000, 012345);

  t.execute_double<op::Mov>(IndexDeferred, 1, Register, 0, 4);

  t.expect_reg(0, 012345);
  t.expect_reg(1, DATA);
  t.expect_pc(01004);
}
