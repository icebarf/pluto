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
#include <array>
#include <cstdint>

// KA11 CPU Architecture

enum RegisterNames
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

struct pdp11
{
  std::array<uint16_t, 9> registers;
};

enum Addressing
{
  Direct,
  Indirect,
};

enum Mode
{
  AutoIncrement,
  AutoDecrement,
  Index
};

struct operand
{
  uint16_t opcode;
  uint8_t destination;
  Addressing addressing;
  Mode mode;
};
