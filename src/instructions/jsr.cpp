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
  uint8_t reg = (instruction.word >> 5) & 0x07;     // extract bit 6-8
  uint16_t destination = instruction.word & 0x001f; // extract all lower 5 bits

  uint16_t temp = read_word(memory, destination);
  registers[stack_pointer] -= 2;
  write_word(memory, registers[stack_pointer], registers[reg]);
  registers[reg] = registers[program_counter];
  registers[program_counter] = temp;
}