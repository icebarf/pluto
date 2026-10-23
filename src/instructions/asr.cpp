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
set_status(StatusRegister status, uint16_t value, bool is_byte)
{
  value == 0 ? status.set_zero() : status.unset_zero();

  int negative = 0;
  int carry = static_cast<int>(status.get_carry());
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

  bool overflow = carry ^ negative;
  overflow ? status.set_overflow() : status.unset_overflow();
}

void
Instructions::asr(MemoryT& memory,
                  RegistersT& registers,
                  Instruction instruction)
{
  uint16_t reg_index = instruction.destination[0];
  bool is_byte = (instruction.dest_type == DestinationType::Byte);
  Instructions::Mode mode = instruction.mode[0];
  StatusRegister state(registers[Registers::status]);

  uint16_t value = 0;
  uint8_t msb = 0;
  uint8_t lsb = 0;
  value = get_operand_value(memory, registers, reg_index, mode, is_byte);
  lsb = value & 0b1;
  if (is_byte) {
    msb = (value >> 7) & 0b1;
  } else {
    msb = (value >> 15) & 0b1;
  }

  value >>= 1;
  if (msb) { // replicate bit-15/7
    if (is_byte) {
      value = value | (1 << 7);
    } else {
      value = value | (1 << 15);
    }
  }

  set_operand_value(value, memory, registers, reg_index, mode, is_byte);

  if (lsb) {
    state.set_carry();
  } else {
    state.unset_carry();
  }
  set_status(state, value, is_byte);
}