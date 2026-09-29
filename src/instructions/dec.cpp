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
  value == 0 ? status.set_zero() : status.unset_zero();
  if (is_byte) {
    value >= 0o200 ? status.set_overflow()
                   : status.unset_overflow(); // 0o200 == 128
  } else {
    value >= 0o100000 ? status.set_overflow()
                      : status.unset_overflow(); // 0o100000 == 32768
  }
}

void
dec(MemoryT& memory, RegistersT& registers, Instruction instruction)
{
  uint16_t reg_index = instruction.destination[0];
  bool is_byte_target = (instruction.dest_type == DestinationType::Byte);
  StatusRegister state(registers[Registers::status]);

  if (instruction.mode[0] == Mode::Register) {
    if (is_byte_target) {
      uint8_t low_byte = registers[reg_index] & 0x00ff;
      low_byte--;
      registers[reg_index] = (registers[reg_index] & 0xff) | low_byte;
      set_status(state, low_byte, true);
    } else {
      registers[reg_index]--;
      uint16_t result = registers[reg_index];
      set_status(state, result, false);
    }
  } else {

    uint16_t target_address = get_operand_address(
      memory, registers, instruction.mode[0], reg_index, is_byte_target);

    if (is_byte_target) {
      uint8_t current = read_byte(memory, target_address);
      current--;
      write_byte(memory, target_address, current);
      set_status(state, current, true);
    } else {
      uint16_t current = read_word(memory, target_address);
      current--;
      write_word(memory, target_address, current);
      set_status(state, current, false);
    }
  }
}