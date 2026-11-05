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
Instructions::jmp(Types::MemoryT& memory,
                  Types::RegistersT& registers,
                  Instruction instruction)
{

  // special case
  const uint8_t reg = instruction.destination[0];
  if (instruction.mode[0] == Mode::AutoIncrement) {
    registers[reg] += 2;
    registers[program_counter] = registers[reg];
    return;
  }

  Operand destination = resolve_operand(
    memory, registers, instruction.destination[0], instruction.mode[0], false);

  uint16_t pc;
  if (destination.is_register) {
    pc = registers[destination.reg];
  } else {
    pc = destination.address;
  }
  registers[program_counter] = pc;
}