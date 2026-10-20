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

  value >> 15 ? status.set_carry() : status.unset_carry();
  int zero = 0;
  value == 0 ? (zero = 1), status.set_zero() : status.unset_zero();

  int negative = 0;
  if (is_byte) {
    // force masking instead of just checking value is below zero because i dont
    // want weird issues where bits spill over into the 16-bit int when
    // operating on bytes, this just feels safe.
    if ((value & 0xff) & 0b1000'0000) {
      negative = 1;
    }
  } else {
    if (value & 0b1000'0000'0000'0000) {
      negative = 1;
    }
  }

  negative ? status.set_negative() : status.unset_negative();

  bool overflow = zero ^ negative;
  overflow ? status.set_overflow() : status.unset_overflow();
}

void
Instructions::asl(MemoryT& memory,
                  RegistersT& registers,
                  Instruction instruction)
{
  uint16_t reg_index = instruction.destination[0];
  bool is_byte_target = (instruction.dest_type == DestinationType::Byte);
  StatusRegister state(registers[Registers::status]);

  uint16_t value = 0;
  uint8_t msb = 0;
  if (instruction.mode[0] == Mode::Register) {
    if (is_byte_target) {
      value = (registers[reg_index] & 0x00ff) << 1;
      registers[reg_index] = (registers[reg_index] & 0xff00) | value;
    } else {
      value = registers[reg_index] << 1;

      registers[reg_index] = value;
    }
  } else {

    uint16_t target_address = get_operand_address(
      memory, registers, instruction.mode[0], reg_index, is_byte_target);

    if (is_byte_target) {
      value = read_byte(memory, target_address) << 1;
      value <<= 1;
      write_byte(memory, target_address, value);
    } else {
      value = read_word(memory, target_address) << 1;
      write_word(memory, target_address, value);
    }
  }
  set_status(state, value, is_byte_target);
}