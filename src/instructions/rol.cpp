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

  bool negative = status.get_negative();
  bool carry = status.get_carry();
  bool overflow = negative ^ carry;
  overflow ? status.set_overflow() : status.unset_overflow();
}

void
Instructions::rol(MemoryT& memory,
                  RegistersT& registers,
                  Instruction instruction)
{
  uint16_t reg_index = instruction.destination[0];
  bool is_byte = (instruction.dest_type == DestinationType::Byte);
  Instructions::Mode mode = instruction.mode[0];
  StatusRegister state(registers[Registers::status]);

  uint16_t value = 0;
  uint16_t msb = 0;
  uint16_t shift_value =
    7; // default is loaded around bytes and not 16-bit values
  uint16_t bitmask = 0b1;

  value = get_operand_value(memory, registers, reg_index, mode, is_byte);
  if (!is_byte) {
    shift_value = 15;
  }
  msb = (value >> shift_value) & 0b1;
  value <<= 1;
  if (state.get_carry()) {
    value |= bitmask;
  }
  set_operand_value(value, memory, registers, reg_index, mode, is_byte);

  if (msb) {
    state.set_carry();
  } else {
    state.unset_carry();
  }
  set_status(state, value, is_byte);
}