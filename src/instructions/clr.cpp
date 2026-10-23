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

using namespace Instructions;
using namespace Types;

void
Instructions::clr(MemoryT& memory,
                  RegistersT& registers,
                  Instruction instruction)
{
  uint16_t reg_index = instruction.destination[0];
  bool is_byte = instruction.dest_type == DestinationType::Byte;
  Instructions::Mode mode = instruction.mode[0];
  Operand operand =
    resolve_operand(memory, registers, reg_index, mode, is_byte);
  write_operand(memory, registers, operand, 0, is_byte);

  StatusRegister state(registers[Registers::status]);
  state.unset_carry();
  state.unset_overflow();
  state.set_zero();
  state.unset_negative();
}