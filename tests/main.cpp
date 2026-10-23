/*
 * Test runner for the PDP-11/20 single-operand instruction tests.
 */
#include "test_framework.hpp"

#include <cstddef>
#include <cstdio>

int
main()
{
  std::size_t passed = 0;
  std::size_t failed = 0;

  for (const auto& test : test::registry()) {
    const int failures_before = test::failures;
    test.fn();

    if (test::failures == failures_before) {
      std::printf("[ ok ] %s\n", test.name);
      ++passed;
    } else {
      std::printf("[FAIL] %s\n", test.name);
      ++failed;
    }
  }

  std::printf("\n%zu passed, %zu failed, %zu total\n",
              passed,
              failed,
              passed + failed);
  return failed == 0 ? 0 : 1;
}
