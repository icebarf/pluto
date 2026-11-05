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

template<void (*Fn)(Types::MemoryT&,
                    Types::RegistersT&,
                    Instructions::Instruction),
         std::uint16_t Opcode>
struct DoubleOperandInstruction
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

template<void (*Fn)(Types::MemoryT&,
                    Types::RegistersT&,
                    Instructions::Instruction),
         std::uint16_t Word>
struct WordInstruction
{
  static constexpr std::uint16_t word = Word;

  static void
  run(Types::MemoryT& memory,
      Types::RegistersT& registers,
      Instructions::Instruction instruction)
  {
    Fn(memory, registers, instruction);
  }
};

template<void (*Fn)(Types::MemoryT&,
                    Types::RegistersT&,
                    Instructions::Instruction),
         std::uint16_t Base>
struct BranchInstruction
{
  static constexpr std::uint16_t base = Base;

  static void
  run(Types::MemoryT& memory,
      Types::RegistersT& registers,
      Instructions::Instruction instruction)
  {
    Fn(memory, registers, instruction);
  }
};

namespace op {
using Mov = DoubleOperandInstruction<&Instructions::mov, 001>;
using Add = DoubleOperandInstruction<&Instructions::add, 006>;
using Sub = DoubleOperandInstruction<&Instructions::sub, 016>;
using Cmp = DoubleOperandInstruction<&Instructions::cmp, 002>;
using Bis = DoubleOperandInstruction<&Instructions::bis, 005>;
using Bit = DoubleOperandInstruction<&Instructions::bit, 003>;
using Bic = DoubleOperandInstruction<&Instructions::bic, 004>;

using Clc = WordInstruction<&Instructions::clc, 000241>;
using Sec = WordInstruction<&Instructions::sec, 000261>;
using Clv = WordInstruction<&Instructions::clv, 000242>;
using Sev = WordInstruction<&Instructions::sev, 000262>;
using Clz = WordInstruction<&Instructions::clz, 000244>;
using Sez = WordInstruction<&Instructions::sez, 000264>;
using Cln = WordInstruction<&Instructions::cln, 000250>;
using Sen = WordInstruction<&Instructions::sen, 000270>;
using Clvc = WordInstruction<&Instructions::clvc, 000243>;
using Clcc = WordInstruction<&Instructions::clcc, 000257>;
using Secc = WordInstruction<&Instructions::secc, 000277>;

using Rti = WordInstruction<&Instructions::rti, 000002>;
using Emt = WordInstruction<&Instructions::emt, static_cast<std::uint16_t>(0104000)>;
using Trap = WordInstruction<&Instructions::trap, static_cast<std::uint16_t>(0104400)>;
using Bpt = WordInstruction<&Instructions::trap_3, 000003>;
using Iot = WordInstruction<&Instructions::iot, 000004>;
} // namespace op

namespace branch {
using Br = BranchInstruction<&Instructions::br, 000400>;
using Bne = BranchInstruction<&Instructions::bne, 001000>;
using Beq = BranchInstruction<&Instructions::beq, 001400>;
using Bge = BranchInstruction<&Instructions::bge, 002000>;
using Blt = BranchInstruction<&Instructions::blt, 002400>;
using Bgt = BranchInstruction<&Instructions::bgt, 003000>;
using Ble = BranchInstruction<&Instructions::ble, 003400>;
using Bpl = BranchInstruction<&Instructions::bpl, static_cast<std::uint16_t>(0100000)>;
using Bmi = BranchInstruction<&Instructions::bmi, static_cast<std::uint16_t>(0100400)>;
using Bhi = BranchInstruction<&Instructions::bhi, static_cast<std::uint16_t>(0101000)>;
using Blos = BranchInstruction<&Instructions::blos, static_cast<std::uint16_t>(0101400)>;
using Bvc = BranchInstruction<&Instructions::bvc, static_cast<std::uint16_t>(0102000)>;
using Bvs = BranchInstruction<&Instructions::bvs, static_cast<std::uint16_t>(0102400)>;
using Bcc = BranchInstruction<&Instructions::bcc, static_cast<std::uint16_t>(0103000)>;
using Bcs = BranchInstruction<&Instructions::bcs, static_cast<std::uint16_t>(0103400)>;
} // namespace branch

template<class Op>
concept DoubleOperandOp = SingleOperandOp<Op>;

template<class Op>
concept WordInstructionOp =
  requires(Types::MemoryT& memory,
           Types::RegistersT& registers,
           Instructions::Instruction instruction) {
    { Op::word } -> std::convertible_to<std::uint16_t>;
    { Op::run(memory, registers, instruction) } -> std::same_as<void>;
  };

template<class Op>
concept BranchInstructionOp =
  requires(Types::MemoryT& memory,
           Types::RegistersT& registers,
           Instructions::Instruction instruction) {
    { Op::base } -> std::convertible_to<std::uint16_t>;
    { Op::run(memory, registers, instruction) } -> std::same_as<void>;
  };

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

  static constexpr std::uint16_t
  encode_double(std::uint16_t opcode,
                Instructions::Mode source_mode,
                std::uint8_t source_reg,
                Instructions::Mode destination_mode,
                std::uint8_t destination_reg,
                bool is_byte) noexcept
  {
    return static_cast<std::uint16_t>((is_byte ? 0100000 : 0) |
                                      (opcode << 12) |
                                      (std::to_underlying(source_mode) << 9) |
                                      (source_reg << 6) |
                                      (std::to_underlying(destination_mode) << 3) |
                                      destination_reg);
  }

  template<DoubleOperandOp Op,
           bool IsByte = false,
           std::convertible_to<std::uint16_t>... Trailing>
  void
  execute_double(Instructions::Mode source_mode,
                 std::uint8_t source_reg,
                 Instructions::Mode destination_mode,
                 std::uint8_t destination_reg,
                 Trailing... trailing)
  {
    static_assert(!IsByte ||
                    (Op::opcode != 006 && Op::opcode != 016 && Op::opcode != 002),
                  "ADD/SUB/CMP byte forms are not implemented");

    const std::uint16_t word = encode_double(Op::opcode,
                                             source_mode,
                                             source_reg,
                                             destination_mode,
                                             destination_reg,
                                             IsByte);

    std::uint16_t address = CODE;
    write_word(memory, address, word);
    address += 2;
    ((write_word(memory, address, static_cast<std::uint16_t>(trailing)),
      address += 2),
     ...);

    registers[Types::program_counter] = CODE + 2;

    const Instructions::Instruction instruction = Instructions::extract(word);
    Op::run(memory, registers, instruction);
  }

  template<WordInstructionOp Op>
  void
  execute_word()
  {
    const std::uint16_t word = Op::word;

    write_word(memory, CODE, word);
    registers[Types::program_counter] = CODE + 2;

    const Instructions::Instruction instruction = Instructions::extract(word);
    Op::run(memory, registers, instruction);
  }

  template<BranchInstructionOp Op>
  void
  execute_branch(std::int8_t offset)
  {
    const std::uint16_t word = static_cast<std::uint16_t>(
      Op::base | (static_cast<std::uint16_t>(offset) & 0x00ff));

    write_word(memory, CODE, word);
    registers[Types::program_counter] = CODE + 2;

    const Instructions::Instruction instruction = Instructions::extract(word);
    Op::run(memory, registers, instruction);
  }

  template<std::convertible_to<std::uint16_t>... Trailing>
  void
  execute_jmp(Instructions::Mode mode,
              std::uint8_t reg_index,
              Trailing... trailing)
  {
    const std::uint16_t word = static_cast<std::uint16_t>(
      000100 | (std::to_underlying(mode) << 3) | reg_index);

    std::uint16_t address = CODE;
    write_word(memory, address, word);
    address += 2;
    ((write_word(memory, address, static_cast<std::uint16_t>(trailing)),
      address += 2),
     ...);

    registers[Types::program_counter] = CODE + 2;

    const Instructions::Instruction instruction = Instructions::extract(word);
    Instructions::jmp(memory, registers, instruction);
  }

  template<std::convertible_to<std::uint16_t>... Trailing>
  void
  execute_jsr(std::uint8_t linkage_reg,
              Instructions::Mode destination_mode,
              std::uint8_t destination_reg,
              Trailing... trailing)
  {
    const std::uint16_t word = static_cast<std::uint16_t>(
      004000 | (linkage_reg << 6) |
      (std::to_underlying(destination_mode) << 3) | destination_reg);

    std::uint16_t address = CODE;
    write_word(memory, address, word);
    address += 2;
    ((write_word(memory, address, static_cast<std::uint16_t>(trailing)),
      address += 2),
     ...);

    // The JSR implementation consumes any destination-address words itself.
    // It stores the resulting return address in the linkage register.
    registers[Types::program_counter] = CODE + 2;

    const Instructions::Instruction instruction = Instructions::extract(word);
    Instructions::jsr(memory, registers, instruction);
  }

  void
  execute_rts(std::uint8_t reg_index)
  {
    const std::uint16_t word =
      static_cast<std::uint16_t>(000200 | reg_index);

    write_word(memory, CODE, word);
    registers[Types::program_counter] = CODE + 2;

    const Instructions::Instruction instruction = Instructions::extract(word);
    Instructions::rts(memory, registers, instruction);
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

  template<DoubleOperandOp Op, bool IsByte = false, class... Trailing>
  void
  execute_double(Instructions::Mode source_mode,
                 std::uint8_t source_reg,
                 Instructions::Mode destination_mode,
                 std::uint8_t destination_reg,
                 Trailing... trailing)
  {
    machine.execute_double<Op, IsByte>(source_mode,
                                       source_reg,
                                       destination_mode,
                                       destination_reg,
                                       trailing...);
  }

  template<WordInstructionOp Op>
  void
  execute_word()
  {
    machine.execute_word<Op>();
  }

  template<BranchInstructionOp Op>
  void
  execute_branch(std::int8_t offset)
  {
    machine.execute_branch<Op>(offset);
  }

  template<class... Trailing>
  void
  execute_jmp(Instructions::Mode mode,
              std::uint8_t reg_index,
              Trailing... trailing)
  {
    machine.execute_jmp(mode, reg_index, trailing...);
  }

  template<class... Trailing>
  void
  execute_jsr(std::uint8_t linkage_reg,
              Instructions::Mode destination_mode,
              std::uint8_t destination_reg,
              Trailing... trailing)
  {
    machine.execute_jsr(
      linkage_reg, destination_mode, destination_reg, trailing...);
  }

  void
  execute_rts(std::uint8_t reg_index)
  {
    machine.execute_rts(reg_index);
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

// A branch case: flags before the branch, signed word offset, and expected PC.
// Branch instructions must not modify the condition codes.
struct BranchCase
{
  std::string_view name;
  Flags psw{};
  std::int8_t offset{};
  std::uint16_t expected_pc{};
};

template<class Suite, std::size_t Index>
void
run_branch_case(Context& t)
{
  const BranchCase& c = Suite::cases[Index];

  t.reset(c.psw);
  t.execute_branch<typename Suite::Op>(c.offset);
  t.expect_pc(c.expected_pc, c.name);
  t.expect_flags(c.psw, c.name);
}

template<class Suite, std::size_t... Indices>
void
register_branch_suite_impl(std::index_sequence<Indices...>)
{
  (registry().add(Suite::cases[Indices].name,
                  &run_branch_case<Suite, Indices>),
   ...);
}

template<class Suite>
void
register_branch_suite()
{
  register_branch_suite_impl<Suite>(
    std::make_index_sequence<Suite::cases.size()>{});
}

template<class Suite>
struct BranchSuiteRegistrar
{
  BranchSuiteRegistrar() { register_branch_suite<Suite>(); }
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

#define REGISTER_BRANCH_SUITE(SuiteType)                                       \
  [[maybe_unused]] static const ::test::BranchSuiteRegistrar<SuiteType>         \
    SuiteType##_registrar

#endif
