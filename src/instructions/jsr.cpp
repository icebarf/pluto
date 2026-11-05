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
#include "../utilities.hpp"
#include "instructions.hpp"
#include <cassert>
#include <cstdint>

using namespace Instructions;
using namespace Types;

void
Instructions::jsr(Types::MemoryT& memory,
                  Types::RegistersT& registers,
                  Instruction instruction)
{
  uint8_t link_reg = (instruction.word >> 6) & 0x07; // extract bit 6-8
  uint8_t reg = instruction.destination[0];

  Operand destination;
  if (instruction.mode[0] == Mode::AutoIncrement) {
    registers[reg] += 2;
    destination = Operand{ false, reg, registers[reg] };
  } else {
    destination =
      resolve_operand(memory, registers, reg, instruction.mode[0], false);
  }

  registers[stack_pointer] -= 2;
  write_word(memory, registers[stack_pointer], registers[link_reg]);
  registers[link_reg] = registers[program_counter];
  if (destination.is_register) {
    registers[program_counter] = registers[destination.reg];
  } else {
    registers[program_counter] = destination.address;
  }
}