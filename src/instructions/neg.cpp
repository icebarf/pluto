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
set_status(StatusRegister status, uint16_t value, bool is_byte)
{
  int16_t signed_value = value;

  signed_value < 0 ? status.set_negative() : status.unset_negative();

  value == 0 ? status.set_zero(), status.unset_carry() : status.unset_zero(),
    status.set_carry();

  if (is_byte) {
    (value & 0xff) >= signed_byte_max_octal
      ? status.set_overflow()
      : status.unset_overflow(); // signed_byte_max_octal == 128
  } else {
    value >= signed_word_max_octal
      ? status.set_overflow()
      : status.unset_overflow(); // signed_word_max_octal == 32768
  }
}

void
Instructions::neg(MemoryT& memory,
                  RegistersT& registers,
                  Instruction instruction)
{
  uint16_t reg_index = instruction.destination[0];
  bool is_byte_target = (instruction.dest_type == DestinationType::Byte);
  StatusRegister state(registers[Registers::status]);

  uint16_t value = 0;
  if (instruction.mode[0] == Mode::Register) {
    if (is_byte_target) {
      value = registers[reg_index] & 0x00ff;
      if (value != signed_byte_max_octal) {
        registers[reg_index] = (registers[reg_index] & 0xff00) | -value;
        value = -value;
      }
    } else {
      value = registers[reg_index];
      if (registers[reg_index] != signed_word_max_octal) {
        registers[reg_index] = -value;
        value = -value;
      }
    }
  } else {

    uint16_t target_address = get_operand_address(
      memory, registers, instruction.mode[0], reg_index, is_byte_target);

    if (is_byte_target) {
      value = read_byte(memory, target_address);
      value = -value;
      write_byte(memory, target_address, value);
    } else {
      value = read_word(memory, target_address);
      value = -value;
      write_word(memory, target_address, value);
    }
  }
  set_status(state, value, is_byte_target);
}