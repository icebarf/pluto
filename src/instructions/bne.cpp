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
#include "instructions.hpp"
#include <cassert>
#include <cstdint>

using namespace Instructions;
using namespace Types;

void
Instructions::bne(Types::MemoryT& memory,
                  Types::RegistersT& registers,
                  Instruction instruction)
{
  if (StatusRegister(registers[Registers::status]).get_zero() == true) {
    return;
  }
  // branch if zero is unset
  int8_t offset = static_cast<int8_t>(instruction.word & 0x00ff);
  registers[Registers::program_counter] += (offset * 2);
}