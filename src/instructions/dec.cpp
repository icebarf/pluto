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

[[nodiscard]] StatusRegister
do_status_checks(uint16_t value)
{
  StatusRegister state{
    // target enablers
    .targets.overflow = true,
    .targets.zero = true,
    .targets.negative = true,
    .targets.carry = false,
    .targets.trap = false,
    .targets.priority = false,
    // actual values
    .overflow = false,
    .zero = false,
    .negative = false,
  };
  int16_t signed_value = value;

  signed_value < 0 ? state.negative = true : state.negative = false;
  value == 0 ? state.zero = true : state.zero = false;
  value >= 0o200 ? state.overflow = true
                 : state.overflow = false; // 0o200 == 128

  return state;
}

void
dec(MemoryT& memory, RegistersT& registers, Instruction instruction)
{
  uint16_t reg_index = instruction.destination[0];
  bool is_byte_target = (instruction.dest_type == DestinationType::Byte);
  StatusRegister state;

  if (instruction.mode[0] == Mode::Register) {
    if (is_byte_target) {
      uint8_t low_byte = registers[reg_index] & 0x00ff;
      low_byte--;
      registers[reg_index] = (registers[reg_index] & 0xff) | low_byte;
      state = do_status_checks(low_byte);
    } else {
      registers[reg_index]--;
      uint16_t result = registers[reg_index];
      state = do_status_checks(result);
    }
  } else {

    uint16_t target_address = get_operand_address(
      memory, registers, instruction.mode[0], reg_index, is_byte_target);

    if (is_byte_target) {
      uint8_t current = read_byte(memory, target_address);
      current--;
      write_byte(memory, target_address, current);
      state = do_status_checks(current);
    } else {
      uint16_t current = read_word(memory, target_address);
      current--;
      write_word(memory, target_address, current);
      state = do_status_checks(current);
    }
  }

  set_status(state, registers[Registers::status]);
}