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
};

enum Mode
{
  // For general registers && SP (Direct)
  Register = 0b0,
  AutoIncrement = 0b0000'0010,
  AutoDecrement = 0b0000'0100,
  Index = 0b0000'0110,
  // For general registers && SP (Indirect)
  RegisterDeferred = 0b1,
  AutoIncrementDeferred = 0b0000'0011,
  AutoDecrementDeferred = 0b0000'0101,
  IndexDeferred = 0b0000'0111,
  // FOR PC
  Immediate = 0b0000'0010,
  Absolute = 0b0000'0011,
  Relative = 0b0000'0110,
  RelativeDeferred = 0b0000'0111,
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
  Registers
    destination[2]; // for double operands: [0] -> destination, [1] -> source
  DestinationType dest_type;
  Mode mode[2]; // [0] -> destination, [1] -> source
};

Instruction
extract(const uint16_t word);

uint16_t
get_operand_address(const Types::MemoryT& memory,
                    Types::RegistersT& registers,
                    Mode mode,
                    uint8_t reg,
                    bool is_byte);

[[nodiscard]] uint16_t
get_operand_value(Types::MemoryT& memory,
                  Types::RegistersT& registers,
                  uint8_t reg_index,
                  Instructions::Mode mode,
                  bool is_byte);

void
set_operand_value(uint16_t value,
                  Types::MemoryT& memory,
                  Types::RegistersT& registers,
                  uint8_t reg_index,
                  Instructions::Mode mode,
                  bool is_byte);

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
