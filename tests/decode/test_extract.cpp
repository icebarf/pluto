/*
 * Decode tests for the single-operand instruction format.
 *
 * Handbook reference: 3.1 Single Operand Addressing, page 20.
 *   bits 15..6 : opcode (bit 15 is the byte/word selector)
 *   bits 5..3  : mode
 *   bits 2..0  : register
 */
#include "test_framework.hpp"

using namespace test;
using enum Instructions::Mode;

TEST(extract_clr_word)
{
  const auto in = Instructions::extract(05000); // CLR R0

  t.expect_eq(in.opcode, 050, "CLR opcode");
  t.expect_true(!in.is_double, "CLR is single");
  t.expect_true(in.dest_type == Instructions::DestinationType::Word,
                "CLR is word");
  t.expect_true(in.destination[0] == 0, "CLR destination register");
  t.expect_true(in.mode[0] == Register, "CLR mode");
}

TEST(extract_clrb_is_byte)
{
  const auto in = Instructions::extract(0105000); // CLRB R0

  t.expect_eq(in.opcode, 050, "CLRB opcode");
  t.expect_true(!in.is_double, "CLRB is single");
  t.expect_true(in.dest_type == Instructions::DestinationType::Byte,
                "CLRB is byte");
  t.expect_true(in.destination[0] == 0, "CLRB destination register");
  t.expect_true(in.mode[0] == Register, "CLRB mode");
}

TEST(extract_swab)
{
  const auto in = Instructions::extract(0000300); // SWAB R0

  t.expect_eq(in.opcode, 003, "SWAB opcode");
  t.expect_true(!in.is_double, "SWAB is single");
  t.expect_true(in.dest_type == Instructions::DestinationType::Word,
                "SWAB is word");
  t.expect_true(in.destination[0] == 0, "SWAB destination register");
  t.expect_true(in.mode[0] == Register, "SWAB mode");
}

TEST(extract_inc_autoincrement)
{
  const auto in = Instructions::extract(
    Machine::encode(052, AutoIncrement, 3, false));

  t.expect_eq(in.opcode, 052, "INC opcode");
  t.expect_true(!in.is_double, "INC is single");
  t.expect_true(in.dest_type == Instructions::DestinationType::Word,
                "INC is word");
  t.expect_true(in.destination[0] == 3, "INC destination register");
  t.expect_true(in.mode[0] == AutoIncrement, "INC mode");
}

TEST(extract_aslb_index)
{
  const auto in = Instructions::extract(
    Machine::encode(063, Index, 2, true));

  t.expect_eq(in.opcode, 063, "ASLB opcode");
  t.expect_true(!in.is_double, "ASLB is single");
  t.expect_true(in.dest_type == Instructions::DestinationType::Byte,
                "ASLB is byte");
  t.expect_true(in.destination[0] == 2, "ASLB destination register");
  t.expect_true(in.mode[0] == Index, "ASLB mode");
}

/*
 * Regression guard: SUB is octal 16 (bit 15 set), but is a word instruction.
 * It must not be decoded as a byte instruction. These decode-only tests do not
 * execute the double-operand instructions.
 */
TEST(extract_sub_is_double_word)
{
  const auto in = Instructions::extract(0160000);

  t.expect_eq(in.opcode, 016, "SUB opcode");
  t.expect_true(in.is_double, "SUB is double");
  t.expect_true(in.dest_type == Instructions::DestinationType::Word,
                "SUB is word");
}

TEST(extract_movb_is_double_byte)
{
  const auto in = Instructions::extract(0110000);

  t.expect_eq(in.opcode, 011, "MOVB opcode");
  t.expect_true(in.is_double, "MOVB is double");
  t.expect_true(in.dest_type == Instructions::DestinationType::Byte,
                "MOVB is byte");
}

TEST(extract_eis_is_not_double_on_11_20)
{
  const auto in = Instructions::extract(070000); // EIS group (11/45)

  t.expect_eq(in.opcode, 007, "EIS opcode");
  t.expect_true(!in.is_double, "EIS is not double on 11/20");
}
