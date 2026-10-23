/*
 * Minimal test framework for the PDP-11/20 single-operand ISA tests.
 *
 * The project's src/ka11.cpp is not yet a runnable CPU, so TestMachine provides
 * just enough fetch/decode/dispatch to invoke one single-operand instruction.
 * It simulates the instruction fetch by setting PC to CODE + 2 before calling
 * the instruction, exactly as a real fetch would.
 */
#pragma once
#ifndef ICE_PDP_TEST_FRAMEWORK_H
#define ICE_PDP_TEST_FRAMEWORK_H

#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <vector>

#include "../src/instructions/instructions.hpp"
#include "../src/types.hpp"
#include "../src/utilities.hpp"

namespace test {

inline int failures = 0;

using TestFn = void (*)();

struct TestCase
{
  const char* name;
  TestFn fn;
};

inline std::vector<TestCase>&
registry()
{
  static std::vector<TestCase> tests;
  return tests;
}

struct Registrar
{
  Registrar(const char* name, TestFn fn) { registry().push_back({ name, fn }); }
};

inline void
fail_message(const char* message)
{
  ++failures;
  std::printf("  %s\n", message);
}

inline void
expect_eq(const char* file,
          int line,
          const char* what,
          unsigned long actual,
          unsigned long expected)
{
  if (actual == expected) {
    return;
  }
  ++failures;
  std::printf("  %s:%d: %s: got 0%lo, expected 0%lo\n",
              file,
              line,
              what,
              actual,
              expected);
}

inline void
expect_true(const char* file, int line, const char* what, bool actual)
{
  if (actual) {
    return;
  }
  ++failures;
  std::printf("  %s:%d: %s: expected true\n", file, line, what);
}

inline void
expect_flags(const char* file,
             int line,
             bool negative,
             bool zero,
             bool overflow,
             bool carry,
             uint16_t psw)
{
  const bool actual_negative = (psw >> 3) & 1;
  const bool actual_zero = (psw >> 2) & 1;
  const bool actual_overflow = (psw >> 1) & 1;
  const bool actual_carry = psw & 1;

  if (actual_negative == negative && actual_zero == zero &&
      actual_overflow == overflow && actual_carry == carry) {
    return;
  }

  ++failures;
  std::printf("  %s:%d: PSW: got N=%d Z=%d V=%d C=%d, "
              "expected N=%d Z=%d V=%d C=%d\n",
              file,
              line,
              actual_negative,
              actual_zero,
              actual_overflow,
              actual_carry,
              negative,
              zero,
              overflow,
              carry);
}

} // namespace test

struct TestMachine
{
  static constexpr uint16_t CODE = 01000;
  static constexpr uint16_t DATA = 02000;
  static constexpr uint16_t TARGET = 03000;

  Types::RegistersT registers{};
  Types::MemoryT memory{};

  void reset(uint16_t psw = 0)
  {
    registers.fill(0);
    memory.fill(0);
    registers[Types::status] = psw;
  }

  // Write `word` at CODE (plus optional operand words following it), move PC
  // past the instruction word, then execute exactly one single-operand
  // instruction.
  void execute(uint16_t word, std::initializer_list<uint16_t> trailing = {})
  {
    uint16_t address = CODE;
    write_word(memory, address, word);
    address += 2;
    for (uint16_t value : trailing) {
      write_word(memory, address, value);
      address += 2;
    }

    // A real fetch leaves PC pointing at the word after the instruction.
    registers[Types::Registers::program_counter] = CODE + 2;

    const Instructions::Instruction instruction =
      Instructions::extract(word);

    if (instruction.is_double) {
      test::fail_message(
        "test machine received a double-operand instruction");
      return;
    }

    switch (instruction.opcode) {
      case 003:
        Instructions::swab(memory, registers, instruction);
        break;
      case 050:
        Instructions::clr(memory, registers, instruction);
        break;
      case 051:
        Instructions::com(memory, registers, instruction);
        break;
      case 052:
        Instructions::inc(memory, registers, instruction);
        break;
      case 053:
        Instructions::dec(memory, registers, instruction);
        break;
      case 054:
        Instructions::neg(memory, registers, instruction);
        break;
      case 055:
        Instructions::adc(memory, registers, instruction);
        break;
      case 056:
        Instructions::sbc(memory, registers, instruction);
        break;
      case 057:
        Instructions::tst(memory, registers, instruction);
        break;
      case 060:
        Instructions::ror(memory, registers, instruction);
        break;
      case 061:
        Instructions::rol(memory, registers, instruction);
        break;
      case 062:
        Instructions::asr(memory, registers, instruction);
        break;
      case 063:
        Instructions::asl(memory, registers, instruction);
        break;
      default:
        test::fail_message("test machine received an unknown opcode");
        break;
    }
  }
};

// Addressing mode field values (bits 5..3 of a single-operand instruction).
enum : uint16_t
{
  MODE_REGISTER = 0,
  MODE_REGISTER_DEFERRED = 1,
  MODE_AUTOINCREMENT = 2,
  MODE_AUTOINCREMENT_DEFERRED = 3,
  MODE_AUTODECREMENT = 4,
  MODE_AUTODECREMENT_DEFERRED = 5,
  MODE_INDEX = 6,
  MODE_INDEX_DEFERRED = 7,
};

// Encode a single-operand instruction. `opcode` is the octal opcode without
// the byte bit (050 CLR, 052 INC, 003 SWAB, ...).
inline uint16_t
single_op(uint16_t opcode, uint16_t mode, uint16_t reg, bool is_byte = false)
{
  return static_cast<uint16_t>((is_byte ? 0100000 : 0) | (opcode << 6) |
                               (mode << 3) | reg);
}

#define TEST(name)                                                             \
  static void name();                                                          \
  static ::test::Registrar name##_registrar(#name, name);                      \
  static void name()

#define EXPECT_EQ(actual, expected)                                            \
  ::test::expect_eq(__FILE__,                                                  \
                    __LINE__,                                                  \
                    #actual,                                                   \
                    static_cast<unsigned long>(actual),                        \
                    static_cast<unsigned long>(expected))

#define EXPECT_TRUE(condition)                                                 \
  ::test::expect_true(__FILE__, __LINE__, #condition, (condition))

#define EXPECT_REG(index, expected)                                            \
  ::test::expect_eq(__FILE__,                                                  \
                    __LINE__,                                                  \
                    "R" #index,                                                \
                    m.registers[index],                                        \
                    static_cast<unsigned long>(expected))

#define EXPECT_PC(expected)                                                    \
  ::test::expect_eq(__FILE__,                                                  \
                    __LINE__,                                                  \
                    "PC",                                                      \
                    m.registers[Types::program_counter],                       \
                    static_cast<unsigned long>(expected))

#define EXPECT_WORD(address, expected)                                         \
  ::test::expect_eq(__FILE__,                                                  \
                    __LINE__,                                                  \
                    "word",                                                    \
                    read_word(m.memory, (address)),                            \
                    static_cast<unsigned long>(expected))

#define EXPECT_BYTE(address, expected)                                         \
  ::test::expect_eq(__FILE__,                                                  \
                    __LINE__,                                                  \
                    "byte",                                                    \
                    read_byte(m.memory, (address)),                            \
                    static_cast<unsigned long>(expected))

#define EXPECT_FLAGS(n, z, v, c)                                               \
  ::test::expect_flags(__FILE__,                                               \
                       __LINE__,                                               \
                       (n),                                                    \
                       (z),                                                    \
                       (v),                                                    \
                       (c),                                                    \
                       m.registers[Types::status])

#endif
