/*
 * Test runner for the PDP-11/20 single-operand tests.
 *
 * One Context is reused for every test; each test resets its machine state.
 */
#include "test_framework.hpp"

#include <cstddef>
#include <cstdio>

int
main()
{
  test::Context context;
  std::size_t passed = 0;
  std::size_t failed = 0;

  for (const auto& test : test::registry().cases()) {
    const int failures_before = context.failures;
    test.fn(context);

    if (context.failures == failures_before) {
      std::printf("[ ok ] %.*s\n",
                  static_cast<int>(test.name.size()),
                  test.name.data());
      ++passed;
    } else {
      std::printf("[FAIL] %.*s\n",
                  static_cast<int>(test.name.size()),
                  test.name.data());
      ++failed;
    }
  }

  std::printf("\n%zu passed, %zu failed, %zu total (%zu checks)\n",
              passed,
              failed,
              passed + failed,
              context.checks);
  return failed == 0 ? 0 : 1;
}
