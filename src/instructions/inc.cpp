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

using namespace Instructions;
using namespace Types;

void
set_status(StatusRegister status,
           uint16_t value,
           uint16_t oldvalue,
           bool is_byte)
{
  int16_t signed_value = value;

  signed_value < 0 ? status.set_negative() : status.unset_negative();
  value == 0 ? status.set_zero() : status.unset_zero();
  if (is_byte) {
    (oldvalue & 0xff) == (signed_byte_max_octal - 1)
      ? status.set_overflow()
      : status.unset_overflow(); // signed_byte_max_octal == 128
  } else {
    oldvalue == (signed_word_max_octal - 1)
      ? status.set_overflow()
      : status.unset_overflow(); // signed_word_max_octal == 32768
  }
}

void
Instructions::inc(MemoryT& memory,
                  RegistersT& registers,
                  Instruction instruction)
{
  uint16_t reg_index = instruction.destination[0];
  bool is_byte = (instruction.dest_type == DestinationType::Byte);
  Instructions::Mode mode = instruction.mode[0];
  StatusRegister state(registers[Registers::status]);

  uint16_t value = 0; // post-operation value
  uint16_t oldvalue = get_operand_value(
    memory, registers, reg_index, mode, is_byte); // pre-operation value
  value = oldvalue + 1;
  set_operand_value(value, memory, registers, reg_index, mode, is_byte);

  set_status(state, value, oldvalue, is_byte);
}