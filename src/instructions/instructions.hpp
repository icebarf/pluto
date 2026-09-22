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
  Registers destination[2];
  DestinationType dest_type;
  Mode mode[2];
};

struct StatusRegister
{
  // to set in the status flag or not
  // true = change; false = do not change
  struct Targets
  {
    bool carry;
    bool overflow;
    bool zero;
    bool negative;
    bool trap;
    bool priority;
  } targets;
  bool carry; // true = set; false = unset
  bool overflow;
  bool zero;
  bool negative;
  bool trap;
  uint8_t priority;
};

Instruction
extract(uint16_t word);

void
set_status(StatusRegister state, uint16_t& status);

uint16_t
get_operand_address(const Types::MemoryT& memory,
                    Types::RegistersT& registers,
                    Mode mode,
                    uint8_t reg,
                    bool is_byte);
// INSTRUCTIONS
// Single Byte Operations
void
clr(Types::MemoryT& memory,
    Types::RegistersT& registers,
    Instruction instruction);

void
dec(Types::MemoryT& memory,
    Types::RegistersT registers,
    Instruction instruction);

} // namespace Instruction

#endif
