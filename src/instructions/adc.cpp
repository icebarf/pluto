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

static void
set_status(StatusRegister status,
           uint16_t value,
           uint16_t oldvalue,
           bool is_byte)
{
  status.set_negative_zero(value, is_byte);
  (oldvalue == 0177777) && status.get_carry()
    ? status.set_carry()
    : status.unset_carry(); // set carry if largest unsigned 16-bit value was
                            // stored before operation

  if (is_byte) {
    ((value & 0xff) == signed_byte_max_octal) && status.get_carry()
      ? status.set_overflow()
      : status.unset_overflow(); // 0o200 == 128
  } else {
    (value == signed_word_max_octal) && status.get_carry()
      ? status.set_overflow()
      : status.unset_overflow(); // 0o100000 == 32768
  }
}

void
Instructions::adc(MemoryT& memory,
                  RegistersT& registers,
                  Instruction instruction)
{
  uint16_t reg_index = instruction.destination[0];
  bool is_byte = (instruction.dest_type == DestinationType::Byte);
  Instructions::Mode mode = instruction.mode[0];
  StatusRegister state(registers[Registers::status]);

  uint16_t value = 0;
  uint16_t oldvalue = 0;

  oldvalue = get_operand_value(memory, registers, reg_index, mode, is_byte);
  if (state.get_carry()) {
    value = oldvalue + 1;
  }
  set_operand_value(value, memory, registers, reg_index, mode, is_byte);
  set_status(state, value, oldvalue, is_byte);
}