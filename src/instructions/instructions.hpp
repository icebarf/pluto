#pragma once
#ifndef ICE_PDP_INSTRUCTIONS_H
#define ICE_PDP_INSTRUCTIONS_H

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

#include <cstdint>

namespace Instruction {

enum Registers
{
  r0 = 0,
  r1 = 1,
  r2 = 2,
  r3 = 3,
  r4 = 4,
  r5 = 5,
  r6 = 6,
  r7 = 7,
  stack_pointer = 6,
  program_counter = 7,
  status = 8
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

struct instruction
{
  uint16_t opcode;
  Registers destination[2];
  DestinationType dest_type;
  Mode mode[2];
};

instruction
extract(uint16_t word);

}; // namespace Instruction

#endif
