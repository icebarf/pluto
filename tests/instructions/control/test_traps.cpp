/*
 * Trap and RTI tests.
 *
 * Handbook pages 93-105. A trap pushes PSW then PC:
 *   SP -= 2; (SP) = PSW; SP -= 2; (SP) = PC;
 *   PC = vector[0]; PSW = vector[1].
 * RTI pops PC then PSW and adds 4 to SP.
 *
 * Vector addresses (octal):
 *   BPT 014/016, IOT 020/022, EMT 030/032, TRAP 034/036.
 */
#include "test_framework.hpp"

using namespace test;

namespace {
constexpr std::uint16_t BPT_VECTOR = 0014;
constexpr std::uint16_t BPT_PSW = 0016;
constexpr std::uint16_t IOT_VECTOR = 0020;
constexpr std::uint16_t IOT_PSW = 0022;
constexpr std::uint16_t EMT_VECTOR = 0030;
constexpr std::uint16_t EMT_PSW = 0032;
constexpr std::uint16_t TRAP_VECTOR = 0034;
constexpr std::uint16_t TRAP_PSW = 0036;
} // namespace

TEST(emt_trap_sequence)
{
  t.reset(flag::c);
  t.reg(6) = DATA;
  t.set_word(EMT_VECTOR, TARGET);
  t.set_word(EMT_PSW, Machine::encode(flag::all));

  t.execute_word<op::Emt>();

  t.expect_reg(6, DATA - 4);
  t.expect_word(DATA - 4, 01002); // pushed PC
  t.expect_word(DATA - 2, Machine::encode(flag::c)); // pushed PSW
  t.expect_pc(TARGET);
  t.expect_flags(flag::all);
}

TEST(trap_trap_sequence)
{
  t.reset(flag::c);
  t.reg(6) = DATA;
  t.set_word(TRAP_VECTOR, TARGET);
  t.set_word(TRAP_PSW, Machine::encode(flag::all));

  t.execute_word<op::Trap>();

  t.expect_reg(6, DATA - 4);
  t.expect_word(DATA - 4, 01002);
  t.expect_word(DATA - 2, Machine::encode(flag::c));
  t.expect_pc(TARGET);
  t.expect_flags(flag::all);
}

TEST(bpt_trap_sequence)
{
  t.reset(flag::c);
  t.reg(6) = DATA;
  t.set_word(BPT_VECTOR, TARGET);
  t.set_word(BPT_PSW, Machine::encode(flag::all));

  t.execute_word<op::Bpt>();

  t.expect_reg(6, DATA - 4);
  t.expect_word(DATA - 4, 01002);
  t.expect_word(DATA - 2, Machine::encode(flag::c));
  t.expect_pc(TARGET);
  t.expect_flags(flag::all);
}

TEST(iot_trap_sequence)
{
  t.reset(flag::c);
  t.reg(6) = DATA;
  t.set_word(IOT_VECTOR, TARGET);
  t.set_word(IOT_PSW, Machine::encode(flag::all));

  t.execute_word<op::Iot>();

  t.expect_reg(6, DATA - 4);
  t.expect_word(DATA - 4, 01002);
  t.expect_word(DATA - 2, Machine::encode(flag::c));
  t.expect_pc(TARGET);
  t.expect_flags(flag::all);
}

TEST(rti_restores_pc_psw_and_advances_sp)
{
  t.reset(flag::c);
  t.reg(6) = DATA;
  t.set_word(DATA, TARGET);
  t.set_word(DATA + 2, Machine::encode(flag::all));

  t.execute_word<op::Rti>();

  t.expect_pc(TARGET);
  t.expect_reg(6, DATA + 4);
  t.expect_flags(flag::all);
}
