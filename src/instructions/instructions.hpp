/*
Copyright (C) 2026 Amritpal Singh

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once
#ifndef ICE_PDP_INSTRUCTIONS_H
#define ICE_PDP_INSTRUCTIONS_H

#include "../types.hpp"
#include <cstdint>

namespace Instructions {

using Types::Registers;

struct StatusRegister
{
private:
  uint16_t& status_register;

public:
  StatusRegister(uint16_t& status);
  void set_carry();
  void unset_carry();
  void set_overflow();
  void unset_overflow();
  void set_zero();
  void unset_zero();
  void set_negative();
  void unset_negative();
  void set_trap();
  void unset_trap();
  void set_priority(int value);

  // getters
  bool get_carry() const;
  bool get_overflow() const;
  bool get_zero() const;
  bool get_negative() const;
  bool get_trap() const;
  int get_priority() const;

  // set flags based on value
  void set_negative_zero(uint16_t value, bool is_byte);
};

enum Mode
{
  // For general registers && SP (Direct)
  Register = 0b0,              // 0
  AutoIncrement = 0b0000'0010, // 2
  AutoDecrement = 0b0000'0100, // 4
  Index = 0b0000'0110,         // 6
  // For general registers && SP (Indirect)
  RegisterDeferred = 0b1,              // 1
  AutoIncrementDeferred = 0b0000'0011, // 3
  AutoDecrementDeferred = 0b0000'0101, // 5
  IndexDeferred = 0b0000'0111,         // 7
  // FOR PC
  Immediate = 0b0000'0010,        // 2
  Absolute = 0b0000'0011,         // 3
  Relative = 0b0000'0110,         // 6
  RelativeDeferred = 0b0000'0111, // 7
};

enum class DestinationType
{
  Byte,
  Word,
};

struct Instruction
{
  uint16_t opcode;
  bool is_double;
  Registers destination[2]; // for double operands: [0] -> destination, [1] ->
                            // source | otherwise [0] is main operand for single
  DestinationType dest_type;
  Mode mode[2]; // [0] -> destination, [1] -> source
};

struct Operand
{
  bool is_register;
  uint8_t reg;
  uint16_t address;
};

struct DoubleOperand
{
  Operand source;
  Operand desitnation;
  uint16_t source_value;
  uint16_t destination_value;
};

Instruction
extract(const uint16_t word);

uint16_t
get_operand_address(const Types::MemoryT& memory,
                    Types::RegistersT& registers,
                    const uint8_t reg,
                    const Mode mode,
                    const bool is_byte);

[[nodiscard]] Operand
resolve_operand(const Types::MemoryT& memory,
                Types::RegistersT& registers,
                const uint8_t reg,
                const Mode mode,
                const bool is_byte);

[[nodiscard]] uint16_t
read_operand(const Types::MemoryT& memory,
             const Types::RegistersT& registers,
             const Operand& operand,
             const bool is_byte);

void
write_operand(Types::MemoryT& memory,
              Types::RegistersT& registers,
              const Operand& operand,
              uint16_t value,
              const bool is_byte);

// INSTRUCTIONS
// Single Byte Operations
void
clr(Types::MemoryT& memory,
    Types::RegistersT& registers,
    Instruction instruction);

void
dec(Types::MemoryT& memory,
    Types::RegistersT& registers,
    Instruction instruction);

void
inc(Types::MemoryT& memory,
    Types::RegistersT& registers,
    Instruction instruction);

void
neg(Types::MemoryT& memory,
    Types::RegistersT& registers,
    Instruction instruction);

void
tst(Types::MemoryT& memory,
    Types::RegistersT& registers,
    Instruction instruction);

void
com(Types::MemoryT& memory,
    Types::RegistersT& registers,
    Instruction instruction);

void
asr(Types::MemoryT& memory,
    Types::RegistersT& registers,
    Instruction instruction);

void
asl(Types::MemoryT& memory,
    Types::RegistersT& registers,
    Instruction instruction);

void
adc(Types::MemoryT& memory,
    Types::RegistersT& registers,
    Instruction instruction);

void
sbc(Types::MemoryT& memory,
    Types::RegistersT& registers,
    Instruction instruction);

void
ror(Types::MemoryT& memory,
    Types::RegistersT& registers,
    Instruction instruction);

void
rol(Types::MemoryT& memory,
    Types::RegistersT& registers,
    Instruction instruction);

void
swab(Types::MemoryT& memory,
     Types::RegistersT& registers,
     Instruction instruction);

} // namespace Instructions

#endif
