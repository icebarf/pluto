/*
 * Branch instruction tests.
 *
 * Handbook pages 69-88:
 *   PC <- PC + 2 * sign_extend(offset) when the condition is true.
 *   PC is the address of the word after the branch instruction (01002 here).
 *   Branch instructions never modify the condition codes.
 */
#include <array>

#include "test_framework.hpp"

using namespace test;

namespace {

struct BrCases
{
  using Op = branch::Br;
  static constexpr auto cases = std::to_array<BranchCase>({
    { "br_forward", flag::none, 1, 01004 },
    { "br_backward", flag::none, -1, 01000 },
  });
};
REGISTER_BRANCH_SUITE(BrCases);

struct BneCases
{
  using Op = branch::Bne;
  static constexpr auto cases = std::to_array<BranchCase>({
    { "bne_taken", flag::none, 1, 01004 },
    { "bne_not_taken", flag::z, 1, 01002 },
  });
};
REGISTER_BRANCH_SUITE(BneCases);

struct BeqCases
{
  using Op = branch::Beq;
  static constexpr auto cases = std::to_array<BranchCase>({
    { "beq_taken", flag::z, 1, 01004 },
    { "beq_backward", flag::z, -1, 01000 },
    { "beq_not_taken", flag::none, 1, 01002 },
  });
};
REGISTER_BRANCH_SUITE(BeqCases);

struct BmiCases
{
  using Op = branch::Bmi;
  static constexpr auto cases = std::to_array<BranchCase>({
    { "bmi_taken", flag::n, 1, 01004 },
    { "bmi_not_taken", flag::none, 1, 01002 },
  });
};
REGISTER_BRANCH_SUITE(BmiCases);

struct BplCases
{
  using Op = branch::Bpl;
  static constexpr auto cases = std::to_array<BranchCase>({
    { "bpl_taken", flag::none, 1, 01004 },
    { "bpl_not_taken", flag::n, 1, 01002 },
  });
};
REGISTER_BRANCH_SUITE(BplCases);

struct BcsCases
{
  using Op = branch::Bcs;
  static constexpr auto cases = std::to_array<BranchCase>({
    { "bcs_taken", flag::c, 1, 01004 },
    { "bcs_not_taken", flag::none, 1, 01002 },
  });
};
REGISTER_BRANCH_SUITE(BcsCases);

struct BccCases
{
  using Op = branch::Bcc;
  static constexpr auto cases = std::to_array<BranchCase>({
    { "bcc_taken", flag::none, 1, 01004 },
    { "bcc_not_taken", flag::c, 1, 01002 },
  });
};
REGISTER_BRANCH_SUITE(BccCases);

struct BvsCases
{
  using Op = branch::Bvs;
  static constexpr auto cases = std::to_array<BranchCase>({
    { "bvs_taken", flag::v, 1, 01004 },
    { "bvs_not_taken", flag::none, 1, 01002 },
  });
};
REGISTER_BRANCH_SUITE(BvsCases);

struct BvcCases
{
  using Op = branch::Bvc;
  static constexpr auto cases = std::to_array<BranchCase>({
    { "bvc_taken", flag::none, 1, 01004 },
    { "bvc_not_taken", flag::v, 1, 01002 },
  });
};
REGISTER_BRANCH_SUITE(BvcCases);

struct BltCases
{
  using Op = branch::Blt;
  static constexpr auto cases = std::to_array<BranchCase>({
    { "blt_taken_n_and_not_v", flag::n, 1, 01004 },
    { "blt_not_taken_n_and_v", flag::n | flag::v, 1, 01002 },
    { "blt_not_taken_zero_signs", flag::none, 1, 01002 },
  });
};
REGISTER_BRANCH_SUITE(BltCases);

struct BgeCases
{
  using Op = branch::Bge;
  static constexpr auto cases = std::to_array<BranchCase>({
    { "bge_taken_zero_signs", flag::none, 1, 01004 },
    { "bge_taken_both_signs", flag::n | flag::v, 1, 01004 },
    { "bge_not_taken_n_only", flag::n, 1, 01002 },
  });
};
REGISTER_BRANCH_SUITE(BgeCases);

struct BleCases
{
  using Op = branch::Ble;
  static constexpr auto cases = std::to_array<BranchCase>({
    { "ble_taken_zero", flag::z, 1, 01004 },
    { "ble_taken_negative", flag::n, 1, 01004 },
    { "ble_not_taken_positive", flag::none, 1, 01002 },
  });
};
REGISTER_BRANCH_SUITE(BleCases);

struct BgtCases
{
  using Op = branch::Bgt;
  static constexpr auto cases = std::to_array<BranchCase>({
    { "bgt_taken_positive", flag::none, 1, 01004 },
    { "bgt_taken_both_signs", flag::n | flag::v, 1, 01004 },
    { "bgt_not_taken_zero", flag::z, 1, 01002 },
    { "bgt_not_taken_negative", flag::n, 1, 01002 },
    { "bgt_not_taken_overflow", flag::v, 1, 01002 },
  });
};
REGISTER_BRANCH_SUITE(BgtCases);

struct BhiCases
{
  using Op = branch::Bhi;
  static constexpr auto cases = std::to_array<BranchCase>({
    { "bhi_taken", flag::none, 1, 01004 },
    { "bhi_not_taken_carry", flag::c, 1, 01002 },
    { "bhi_not_taken_zero", flag::z, 1, 01002 },
  });
};
REGISTER_BRANCH_SUITE(BhiCases);

struct BlosCases
{
  using Op = branch::Blos;
  static constexpr auto cases = std::to_array<BranchCase>({
    { "blos_taken_carry", flag::c, 1, 01004 },
    { "blos_taken_zero", flag::z, 1, 01004 },
    { "blos_not_taken", flag::none, 1, 01002 },
  });
};
REGISTER_BRANCH_SUITE(BlosCases);

} // namespace
