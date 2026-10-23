/*
 * Decode tests for the single-operand instruction format.
 *
 * Handbook reference: 3.1 Single Operand Addressing, page 20.
 *   bits 15..6 : opcode (bit 15 is the byte/word selector)
 *   bits 5..3  : mode
 *   bits 2..0  : register
 */
#include "test_framework.hpp"

TEST(extract_clr_word)
{
  const auto in = Instructions::extract(05000); // CLR R0

  EXPECT_EQ(in.opcode, 050);
  EXPECT_EQ(in.is_double, false);
  EXPECT_TRUE(in.dest_type == Instructions::DestinationType::Word);
  EXPECT_EQ(in.destination[0], 0);
  EXPECT_EQ(in.mode[0], MODE_REGISTER);
}

TEST(extract_clrb_is_byte)
{
  const auto in = Instructions::extract(0105000); // CLRB R0

  EXPECT_EQ(in.opcode, 050);
  EXPECT_EQ(in.is_double, false);
  EXPECT_TRUE(in.dest_type == Instructions::DestinationType::Byte);
  EXPECT_EQ(in.destination[0], 0);
  EXPECT_EQ(in.mode[0], MODE_REGISTER);
}

TEST(extract_swab)
{
  const auto in = Instructions::extract(0000300); // SWAB R0

  EXPECT_EQ(in.opcode, 003);
  EXPECT_EQ(in.is_double, false);
  EXPECT_TRUE(in.dest_type == Instructions::DestinationType::Word);
  EXPECT_EQ(in.destination[0], 0);
  EXPECT_EQ(in.mode[0], MODE_REGISTER);
}

TEST(extract_inc_autoincrement)
{
  const auto in = Instructions::extract(single_op(052, MODE_AUTOINCREMENT, 3));

  EXPECT_EQ(in.opcode, 052);
  EXPECT_EQ(in.is_double, false);
  EXPECT_TRUE(in.dest_type == Instructions::DestinationType::Word);
  EXPECT_EQ(in.destination[0], 3);
  EXPECT_EQ(in.mode[0], MODE_AUTOINCREMENT);
}

TEST(extract_aslb_index)
{
  const auto in =
    Instructions::extract(single_op(063, MODE_INDEX, 2, true));

  EXPECT_EQ(in.opcode, 063);
  EXPECT_EQ(in.is_double, false);
  EXPECT_TRUE(in.dest_type == Instructions::DestinationType::Byte);
  EXPECT_EQ(in.destination[0], 2);
  EXPECT_EQ(in.mode[0], MODE_INDEX);
}

/*
 * Regression guard: SUB is octal 16 (bit 15 set), but is a word instruction.
 * It must not be decoded as a byte instruction. These decode-only tests do not
 * execute the double-operand instructions.
 */
TEST(extract_sub_is_double_word)
{
  const auto in = Instructions::extract(0160000);

  EXPECT_EQ(in.opcode, 016);
  EXPECT_EQ(in.is_double, true);
  EXPECT_TRUE(in.dest_type == Instructions::DestinationType::Word);
}

TEST(extract_movb_is_double_byte)
{
  const auto in = Instructions::extract(0110000);

  EXPECT_EQ(in.opcode, 011);
  EXPECT_EQ(in.is_double, true);
  EXPECT_TRUE(in.dest_type == Instructions::DestinationType::Byte);
}

TEST(extract_eis_is_not_double_on_11_20)
{
  const auto in = Instructions::extract(070000); // EIS group (11/45)

  EXPECT_EQ(in.opcode, 007);
  EXPECT_EQ(in.is_double, false);
}
