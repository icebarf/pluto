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

using namespace Instructions;
using namespace Types;

void
Instructions::iot(Types::MemoryT& memory,
                  Types::RegistersT& registers,
                  Instruction instruction)
{
  registers[stack_pointer] -= 2;
  write_word(memory, registers[stack_pointer], registers[status]);
  registers[stack_pointer] -= 2;
  write_word(memory, registers[stack_pointer], registers[program_counter]);
  registers[program_counter] = read_word(memory, 020);
  registers[status] = read_word(memory, 022);
}