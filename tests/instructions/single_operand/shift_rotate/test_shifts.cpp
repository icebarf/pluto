/*
 * ASL / ASR / ROL / ROR tests (word and byte forms).
 *
 * Handbook references:
 *   ASR page 49: C <- old bit 0; N/Z from result; V = N xor C.
 *   ASL page 50: C <- old bit 15 (bit 7 for byte); N/Z; V = N xor C.
 *   ROL page 55: C <- old bit 15 (bit 7); old C -> bit 0; V = N xor C.
 *   ROR page 56: C <- old bit 0; old C -> bit 15 (bit 7); V = N xor C.
 *
 * For byte variants, N is result bit 7 and Z is the low byte being zero.
 * Register high bytes are preserved.
 */
#include <array>

#include "test_framework.hpp"

using namespace test;

namespace {

struct AslWordCases
{
  using Op = op::Asl;
  static constexpr bool is_byte = false;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "asl_word_one", 1, 2, {}, {} },
    { "asl_word_msb_out", 0100000, 0, {}, flag::z | flag::v | flag::c },
    { "asl_word_overflow", 0040000, 0100000, {}, flag::n | flag::v },
    { "asl_word_carry_and_result", 0100001, 2, {}, flag::v | flag::c },
  });
};
REGISTER_SUITE(AslWordCases);

struct AslByteCases
{
  using Op = op::Asl;
  static constexpr bool is_byte = true;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "aslb_msb_out_preserves_high_byte", 012200, 012000,
      {}, flag::z | flag::v | flag::c },
    { "aslb_overflow", 012100, 012200, {}, flag::n | flag::v },
  });
};
REGISTER_SUITE(AslByteCases);

struct AsrWordCases
{
  using Op = op::Asr;
  static constexpr bool is_byte = false;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "asr_word_four", 4, 2, {}, {} },
    { "asr_word_sign_replicated", 0100000, 0140000, {}, flag::n | flag::v },
    { "asr_word_lsb_out", 1, 0, {}, flag::z | flag::v | flag::c },
    { "asr_word_odd_negative", 0100001, 0140000, {}, flag::n | flag::c },
  });
};
REGISTER_SUITE(AsrWordCases);

struct AsrByteCases
{
  using Op = op::Asr;
  static constexpr bool is_byte = true;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "asrb_sign_replicated_preserves_high_byte", 012200, 012300,
      {}, flag::n | flag::v },
    { "asrb_lsb_out", 012001, 012000, {}, flag::z | flag::v | flag::c },
    { "asrb_odd_negative", 012201, 012300, {}, flag::n | flag::c },
  });
};
REGISTER_SUITE(AsrByteCases);

struct RolWordCases
{
  using Op = op::Rol;
  static constexpr bool is_byte = false;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "rol_word_one", 1, 2, {}, {} },
    { "rol_word_carry_in", 1, 3, flag::c, {} },
    { "rol_word_msb_out", 0100000, 0, {}, flag::z | flag::v | flag::c },
    { "rol_word_msb_out_with_carry_in", 0100001, 3, flag::c, flag::v | flag::c },
  });
};
REGISTER_SUITE(RolWordCases);

struct RolByteCases
{
  using Op = op::Rol;
  static constexpr bool is_byte = true;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "rolb_msb_out_preserves_high_byte", 012200, 012000,
      {}, flag::z | flag::v | flag::c },
    { "rolb_carry_in", 012001, 012003, flag::c, {} },
  });
};
REGISTER_SUITE(RolByteCases);

struct RorWordCases
{
  using Op = op::Ror;
  static constexpr bool is_byte = false;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "ror_word_two", 2, 1, {}, {} },
    { "ror_word_carry_in", 2, 0100001, flag::c, flag::n | flag::v },
    { "ror_word_lsb_out", 1, 0, {}, flag::z | flag::v | flag::c },
    { "ror_word_lsb_out_with_carry_in", 1, 0100000, flag::c, flag::n | flag::c },
  });
};
REGISTER_SUITE(RorWordCases);

struct RorByteCases
{
  using Op = op::Ror;
  static constexpr bool is_byte = true;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "rorb_lsb_out_preserves_high_byte", 012001, 012000,
      {}, flag::z | flag::v | flag::c },
    { "rorb_carry_in", 012001, 012200, flag::c, flag::n | flag::c },
  });
};
REGISTER_SUITE(RorByteCases);

} // namespace
