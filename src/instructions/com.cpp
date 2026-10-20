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

  status.unset_overflow();
  status.set_carry();
  value == 0 ? status.set_zero() : status.unset_zero();

  if (is_byte) {
    if ((value & 0xff) & 0b1000'0000) {
      status.set_negative();
    } else {
      status.unset_negative();
    }
  } else {
    if (signed_value & 0b1000'0000'0000'0000) {
      status.set_negative();
    } else {
      status.unset_negative();
    }
  }
}

void
Instructions::com(MemoryT& memory,
                  RegistersT& registers,
                  Instruction instruction)
{
  uint16_t reg_index = instruction.destination[0];
  bool is_byte_target = (instruction.dest_type == DestinationType::Byte);
  StatusRegister state(registers[Registers::status]);

  uint16_t value = 0;
  if (instruction.mode[0] == Mode::Register) {
    if (is_byte_target) {
      uint8_t comp_low_byte = ~(registers[reg_index] & 0x00ff);
      registers[reg_index] = (registers[reg_index] & 0xff00) | comp_low_byte;
      value = comp_low_byte;
    } else {
      registers[reg_index] = ~registers[reg_index];
      value = registers[reg_index];
    }
  } else {

    uint16_t target_address = get_operand_address(
      memory, registers, instruction.mode[0], reg_index, is_byte_target);

    if (is_byte_target) {
      uint8_t current = read_byte(memory, target_address);
      current = ~current;
      write_byte(memory, target_address, current);
      value = current;
    } else {
      uint16_t current = read_word(memory, target_address);
      current = ~current;
      write_word(memory, target_address, current);
      value = current;
    }
  }
  set_status(state, value, is_byte_target);
}