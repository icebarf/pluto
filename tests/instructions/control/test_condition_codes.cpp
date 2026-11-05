/*
 * Condition-code operator tests.
 *
 * Handbook pages 107-108:
 *   CLC 000241  SEC 000261
 *   CLV 000242  SEV 000262
 *   CLZ 000244  SEZ 000264
 *   CLN 000250  SEN 000270
 *   CLVC 000243 (clear V and C)
 *   CCC 000257 (clear all), SCC 000277 (set all)
 */
#include "test_framework.hpp"

using namespace test;

TEST(clc_clears_only_carry)
{
  t.reset(flag::all);
  t.execute_word<op::Clc>();
  t.expect_flags(flag::n | flag::z | flag::v);
}

TEST(sec_sets_only_carry)
{
  t.reset(flag::none);
  t.execute_word<op::Sec>();
  t.expect_flags(flag::c);
}

TEST(clv_clears_only_overflow)
{
  t.reset(flag::all);
  t.execute_word<op::Clv>();
  t.expect_flags(flag::n | flag::z | flag::c);
}

TEST(sev_sets_only_overflow)
{
  t.reset(flag::none);
  t.execute_word<op::Sev>();
  t.expect_flags(flag::v);
}

TEST(clz_clears_only_zero)
{
  t.reset(flag::all);
  t.execute_word<op::Clz>();
  t.expect_flags(flag::n | flag::v | flag::c);
}

TEST(sez_sets_only_zero)
{
  t.reset(flag::none);
  t.execute_word<op::Sez>();
  t.expect_flags(flag::z);
}

TEST(cln_clears_only_negative)
{
  t.reset(flag::all);
  t.execute_word<op::Cln>();
  t.expect_flags(flag::z | flag::v | flag::c);
}

TEST(sen_sets_only_negative)
{
  t.reset(flag::none);
  t.execute_word<op::Sen>();
  t.expect_flags(flag::n);
}

TEST(clvc_clears_carry_and_overflow)
{
  t.reset(flag::all);
  t.execute_word<op::Clvc>();
  t.expect_flags(flag::n | flag::z);
}

TEST(ccc_clears_all_condition_codes)
{
  t.reset(flag::all);
  t.execute_word<op::Clcc>();
  t.expect_flags(flag::none);
}

TEST(scc_sets_all_condition_codes)
{
  t.reset(flag::none);
  t.execute_word<op::Secc>();
  t.expect_flags(flag::all);
}
