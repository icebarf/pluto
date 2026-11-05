/*
 * ADC / ADCB / SBC / SBCB tests.
 *
 * Handbook references:
 *   ADC page 52:
 *     result = (dst) + C
 *     V set if (dst) was 077777 and C was 1 (0177 for byte)
 *     C set if (dst) was 177777 and C was 1 (0377 for byte)
 *     otherwise C cleared.
 *     With C = 0 the destination is effectively unchanged.
 *   SBC page 53:
 *     result = (dst) - C
 *     V set if result is 100000 (0200 for byte)
 *     C cleared if result is 0 and C was 1; set otherwise.
 *     With C = 0 the destination is effectively unchanged.
 */
#include <array>

#include "test_framework.hpp"

using namespace test;

namespace {

struct AdcWordCases
{
  using Op = op::Adc;
  static constexpr bool is_byte = false;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "adc_word_carry_clear_retains_value", 5, 5, {}, {} },
    { "adc_word_carry_set_adds_one", 5, 6, flag::c, {} },
    { "adc_word_carry_out", 0177777, 0, flag::c, flag::z | flag::c },
    { "adc_word_overflow", 0077777, 0100000, flag::c, flag::n | flag::v },
  });
};
REGISTER_SUITE(AdcWordCases);

struct AdcByteCases
{
  using Op = op::Adc;
  static constexpr bool is_byte = true;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "adc_byte_carry_clear_retains_value", 012005, 012005, {}, {} },
    { "adc_byte_carry_set_adds_one", 012005, 012006, flag::c, {} },
    { "adc_byte_carry_out", 0000377, 0, flag::c, flag::z | flag::c },
    { "adc_byte_overflow", 012177, 012200, flag::c, flag::n | flag::v },
  });
};
REGISTER_SUITE(AdcByteCases);

struct SbcWordCases
{
  using Op = op::Sbc;
  static constexpr bool is_byte = false;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "sbc_word_carry_clear_retains_value", 5, 5, {}, flag::c },
    { "sbc_word_carry_set_subtracts_one", 5, 4, flag::c, flag::c },
    { "sbc_word_result_zero_clears_carry", 1, 0, flag::c, flag::z },
    { "sbc_word_borrow", 0, 0177777, flag::c, flag::n | flag::c },
    { "sbc_word_overflow", 0100001, 0100000, flag::c, flag::n | flag::v | flag::c },
  });
};
REGISTER_SUITE(SbcWordCases);

struct SbcByteCases
{
  using Op = op::Sbc;
  static constexpr bool is_byte = true;

  static constexpr auto cases = std::to_array<RegisterCase>({
    { "sbc_byte_carry_clear_retains_value", 012005, 012005, {}, flag::c },
    { "sbc_byte_carry_set_subtracts_one", 012005, 012004, flag::c, flag::c },
    { "sbc_byte_result_zero_clears_carry", 012001, 012000, flag::c, flag::z },
    { "sbc_byte_borrow", 012000, 012377, flag::c, flag::n | flag::c },
    { "sbc_byte_overflow", 012201, 012200, flag::c, flag::n | flag::v | flag::c },
  });
};
REGISTER_SUITE(SbcByteCases);

} // namespace
