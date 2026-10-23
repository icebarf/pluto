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

static void
set_status(StatusRegister status, uint16_t value, bool is_byte)
{
  status.set_negative_zero(value, is_byte);
  bool negative = status.get_negative();
  bool carry = status.get_carry();
  bool overflow = negative ^ carry;
  overflow ? status.set_overflow() : status.unset_overflow();
}

void
Instructions::ror(MemoryT& memory,
                  RegistersT& registers,
                  Instruction instruction)
{
  uint16_t reg_index = instruction.destination[0];
  bool is_byte = instruction.dest_type == DestinationType::Byte;
  Instructions::Mode mode = instruction.mode[0];
  StatusRegister state(registers[Registers::status]);
  Operand operand =
    resolve_operand(memory, registers, reg_index, mode, is_byte);

  uint16_t value = 0;
  uint16_t lsb = 0;
  uint16_t bitmask = 0b1000'0000;
  uint16_t target_address = 0;

  value = read_operand(memory, registers, operand, is_byte);
  if (!is_byte) {
    bitmask = 0b1000'0000'0000'0000;
  }
  lsb = value & 0b1;
  value >>= 1;
  if (state.get_carry()) {
    value |= bitmask;
  }
  write_operand(memory, registers, operand, value, is_byte);

  if (lsb) {
    state.set_carry();
  } else {
    state.unset_carry();
  }
  set_status(state, value, is_byte);
}