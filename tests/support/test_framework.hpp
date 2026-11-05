/*
 * Template-based test framework for the PDP-11/20 single-operand tests.
 *
 * Design goals:
 *   - Compile-time instruction dispatch via an instruction descriptor type.
 *   - No heap allocation: tests are registered in a fixed-capacity array.
 *   - Reuse one Machine/Context for every test instead of constructing a
 *     56 KiB memory image per test.
 *   - Table-driven register cases expanded at compile time, one registered
 *     test per case, so failures keep their individual names.
 *   - Modern diagnostics via std::source_location and std::string_view.
 */
#pragma once
#ifndef ICE_PDP_TEST_FRAMEWORK_H
#define ICE_PDP_TEST_FRAMEWORK_H

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <source_location>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

#include "instructions/instructions.hpp"
#include "types.hpp"
#include "utilities.hpp"

namespace test {

inline constexpr std::uint16_t CODE = 01000;
inline constexpr std::uint16_t DATA = 02000;
inline constexpr std::uint16_t TARGET = 03000;

struct Flags
{
  bool n{};
  bool z{};
  bool v{};
  bool c{};

  friend constexpr bool operator==(const Flags&, const Flags&) = default;

  friend constexpr Flags
  operator|(Flags lhs, Flags rhs) noexcept
  {
    return Flags{ lhs.n || rhs.n,
                  lhs.z || rhs.z,
                  lhs.v || rhs.v,
                  lhs.c || rhs.c };
  }
};

namespace flag {
inline constexpr Flags none{};
inline constexpr Flags n{ .n = true };
inline constexpr Flags z{ .z = true };
inline constexpr Flags v{ .v = true };
inline constexpr Flags c{ .c = true };
inline constexpr Flags all{ .n = true, .z = true, .v = true, .c = true };
} // namespace flag

struct Context;

using TestFn = void (*)(Context&);

struct TestCase
{
  std::string_view name;
  TestFn fn;
};

class Registry
{
public:
  static constexpr std::size_t capacity = 512;

  constexpr void
  add(std::string_view name, TestFn fn) noexcept
  {
    if (size_ < capacity) {
      cases_[size_++] = TestCase{ name, fn };
    }
  }

  [[nodiscard]] constexpr std::span<const TestCase>
  cases() const noexcept
  {
    return { cases_.data(), size_ };
  }

private:
  std::array<TestCase, capacity> cases_{};
  std::size_t size_{};
};

inline Registry&
registry() noexcept
{
  static Registry instance;
  return instance;
}

template<auto Fn>
struct Registrar
{
  explicit Registrar(std::string_view name) noexcept
  {
    registry().add(name, Fn);
  }
};

template<class Op>
concept SingleOperandOp =
  requires(Types::MemoryT& memory,
           Types::RegistersT& registers,
           Instructions::Instruction instruction) {
    { Op::opcode } -> std::convertible_to<std::uint16_t>;
    { Op::run(memory, registers, instruction) } -> std::same_as<void>;
  };

template<void (*Fn)(Types::MemoryT&,
                    Types::RegistersT&,
                    Instructions::Instruction),
         std::uint16_t Opcode>
struct SingleOperand
{
  static constexpr std::uint16_t opcode = Opcode;

  static void
  run(Types::MemoryT& memory,
      Types::RegistersT& registers,
      Instructions::Instruction instruction)
  {
    Fn(memory, registers, instruction);
  }
};

namespace op {
using Clr = SingleOperand<&Instructions::clr, 050>;
using Com = SingleOperand<&Instructions::com, 051>;
using Inc = SingleOperand<&Instructions::inc, 052>;
using Dec = SingleOperand<&Instructions::dec, 053>;
using Neg = SingleOperand<&Instructions::neg, 054>;
using Adc = SingleOperand<&Instructions::adc, 055>;
using Sbc = SingleOperand<&Instructions::sbc, 056>;
using Tst = SingleOperand<&Instructions::tst, 057>;
using Ror = SingleOperand<&Instructions::ror, 060>;
using Rol = SingleOperand<&Instructions::rol, 061>;
using Asr = SingleOperand<&Instructions::asr, 062>;
using Asl = SingleOperand<&Instructions::asl, 063>;
using Swab = SingleOperand<&Instructions::swab, 003>;
} // namespace op

class Machine
{
public:
  Types::RegistersT registers{};
  Types::MemoryT memory{};

  static constexpr std::uint16_t
  encode(Flags flags) noexcept
  {
    return static_cast<std::uint16_t>((flags.n ? 8 : 0) | (flags.z ? 4 : 0) |
                                      (flags.v ? 2 : 0) | (flags.c ? 1 : 0));
  }

  static constexpr std::uint16_t
  encode(std::uint16_t opcode,
         Instructions::Mode mode,
         std::uint8_t reg,
         bool is_byte) noexcept
  {
    return static_cast<std::uint16_t>((is_byte ? 0100000 : 0) |
                                      (opcode << 6) |
                                      (std::to_underlying(mode) << 3) | reg);
  }

  void
  reset(Flags flags = {})
  {
    registers.fill(0);
    memory.fill(0);
    registers[Types::status] = encode(flags);
  }

  template<SingleOperandOp Op,
           bool IsByte = false,
           std::convertible_to<std::uint16_t>... Trailing>
  void
  execute(Instructions::Mode mode, std::uint8_t reg, Trailing... trailing)
  {
    const std::uint16_t word = encode(Op::opcode, mode, reg, IsByte);

    std::uint16_t address = CODE;
    write_word(memory, address, word);
    address += 2;
    ((write_word(memory, address, static_cast<std::uint16_t>(trailing)),
      address += 2),
     ...);

    // A real fetch leaves PC pointing at the word after the instruction.
    registers[Types::program_counter] = CODE + 2;

    const Instructions::Instruction instruction = Instructions::extract(word);
    Op::run(memory, registers, instruction);
  }
};

class Context
{
public:
  Machine machine;
  int failures = 0;
  std::size_t checks = 0;

  void
  reset(Flags flags = {})
  {
    machine.reset(flags);
  }

  [[nodiscard]] std::uint16_t&
  reg(std::size_t index) noexcept
  {
    return machine.registers[index];
  }

  void
  set_word(std::uint16_t address, std::uint16_t value)
  {
    write_word(machine.memory, address, value);
  }

  void
  set_byte(std::uint16_t address, std::uint8_t value)
  {
    write_byte(machine.memory, address, value);
  }

  template<SingleOperandOp Op, bool IsByte = false, class... Trailing>
  void
  execute(Instructions::Mode mode,
          std::uint8_t reg_index,
          Trailing... trailing)
  {
    machine.execute<Op, IsByte>(mode, reg_index, trailing...);
  }

  void
  expect_eq(std::uint32_t actual,
            std::uint32_t expected,
            std::string_view what,
            std::source_location loc = std::source_location::current())
  {
    ++checks;
    if (actual == expected) {
      return;
    }
    ++failures;
    print_prefix(loc, what);
    std::printf("got 0%lo, expected 0%lo\n",
                static_cast<unsigned long>(actual),
                static_cast<unsigned long>(expected));
  }

  void
  expect_true(bool actual,
              std::string_view what,
              std::source_location loc = std::source_location::current())
  {
    ++checks;
    if (actual) {
      return;
    }
    ++failures;
    print_prefix(loc, what);
    std::printf("expected true\n");
  }

  void
  expect_reg(std::size_t index,
             std::uint16_t expected,
             std::string_view what = {},
             std::source_location loc = std::source_location::current())
  {
    ++checks;
    const std::uint16_t actual = machine.registers[index];
    if (actual == expected) {
      return;
    }
    ++failures;
    print_prefix(loc, what);
    std::printf("R%zu: got 0%lo, expected 0%lo\n",
                index,
                static_cast<unsigned long>(actual),
                static_cast<unsigned long>(expected));
  }

  void
  expect_pc(std::uint16_t expected,
            std::string_view what = "PC",
            std::source_location loc = std::source_location::current())
  {
    expect_reg(Types::program_counter, expected, what, loc);
  }

  void
  expect_word(std::uint16_t address,
              std::uint16_t expected,
              std::string_view what = "word",
              std::source_location loc = std::source_location::current())
  {
    ++checks;
    const std::uint16_t actual = read_word(machine.memory, address);
    if (actual == expected) {
      return;
    }
    ++failures;
    print_prefix(loc, what);
    std::printf("[0%lo]: got 0%lo, expected 0%lo\n",
                static_cast<unsigned long>(address),
                static_cast<unsigned long>(actual),
                static_cast<unsigned long>(expected));
  }

  void
  expect_byte(std::uint16_t address,
              std::uint8_t expected,
              std::string_view what = "byte",
              std::source_location loc = std::source_location::current())
  {
    ++checks;
    const std::uint8_t actual = read_byte(machine.memory, address);
    if (actual == expected) {
      return;
    }
    ++failures;
    print_prefix(loc, what);
    std::printf("[0%lo]: got 0%lo, expected 0%lo\n",
                static_cast<unsigned long>(address),
                static_cast<unsigned long>(actual),
                static_cast<unsigned long>(expected));
  }

  void
  expect_flags(Flags expected,
               std::string_view what = {},
               std::source_location loc = std::source_location::current())
  {
    ++checks;
    const std::uint16_t psw = machine.registers[Types::status];
    const Flags actual{ .n = ((psw >> 3) & 1) != 0,
                        .z = ((psw >> 2) & 1) != 0,
                        .v = ((psw >> 1) & 1) != 0,
                        .c = (psw & 1) != 0 };
    if (actual == expected) {
      return;
    }
    ++failures;
    print_prefix(loc, what);
    std::printf("PSW: got N=%d Z=%d V=%d C=%d, "
                "expected N=%d Z=%d V=%d C=%d\n",
                actual.n,
                actual.z,
                actual.v,
                actual.c,
                expected.n,
                expected.z,
                expected.v,
                expected.c);
  }

private:
  static void
  print_prefix(std::source_location loc, std::string_view what)
  {
    std::printf("  %s:%u: ",
                loc.file_name(),
                static_cast<unsigned>(loc.line()));
    if (!what.empty()) {
      std::printf("%.*s: ",
                  static_cast<int>(what.size()),
                  what.data());
    }
  }
};

// A register-mode case for a single-operand instruction. All cases in a suite
// execute on R0 so the table stays uniform.
struct RegisterCase
{
  std::string_view name;
  std::uint16_t initial;
  std::uint16_t expected;
  Flags psw{};
  Flags expected_flags{};
};

template<class Suite, std::size_t Index>
void
run_register_case(Context& t)
{
  const RegisterCase& c = Suite::cases[Index];

  t.reset(c.psw);
  t.reg(0) = c.initial;
  t.execute<typename Suite::Op, Suite::is_byte>(
    Instructions::Mode::Register, 0);
  t.expect_reg(0, c.expected, c.name);
  t.expect_flags(c.expected_flags, c.name);
}

template<class Suite, std::size_t... Indices>
void
register_suite_impl(std::index_sequence<Indices...>)
{
  (registry().add(Suite::cases[Indices].name,
                  &run_register_case<Suite, Indices>),
   ...);
}

template<class Suite>
void
register_suite()
{
  register_suite_impl<Suite>(
    std::make_index_sequence<Suite::cases.size()>{});
}

template<class Suite>
struct SuiteRegistrar
{
  SuiteRegistrar() { register_suite<Suite>(); }
};

} // namespace test

#define TEST(name)                                                             \
  static void name(::test::Context&);                                          \
  [[maybe_unused]] static const ::test::Registrar<&name> name##_registrar{     \
    #name                                                                      \
  };                                                                           \
  static void name(::test::Context& t)

#define REGISTER_SUITE(SuiteType)                                              \
  [[maybe_unused]] static const ::test::SuiteRegistrar<SuiteType>               \
    SuiteType##_registrar

#endif
