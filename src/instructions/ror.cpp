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

[[nodiscard]] uint16_t
get_operand_value(MemoryT& memory,
                  RegistersT& registers,
                  uint16_t reg_index,
                  Instructions::Mode mode,
                  bool is_byte)
{
  uint16_t value;
  if (mode == Mode::Register) {
    if (is_byte) {
      value = registers[reg_index] & 0x00ff;
    } else {
      value = registers[reg_index];
    }
  } else {
    uint16_t target_address =
      get_operand_address(memory, registers, mode, reg_index, is_byte);
    if (is_byte) {
      value = read_byte(memory, target_address);
    } else {
      value = read_word(memory, target_address);
    }
  }
  return value;
}

void
set_operand_value(uint16_t value,
                  MemoryT& memory,
                  RegistersT& registers,
                  uint16_t reg_index,
                  Instructions::Mode mode,
                  bool is_byte)
{
  if (mode == Mode::Register) {
    if (is_byte) {
      registers[reg_index] = (registers[reg_index] & 0xff00) | value;
    } else {
      registers[reg_index] = value;
    }

  } else {
    uint16_t target_address =
      get_operand_address(memory, registers, mode, reg_index, is_byte);
    if (is_byte) {
      write_byte(memory, target_address, value);
    } else {
      write_word(memory, target_address, value);
    }
  }
}

void
Instructions::ror(MemoryT& memory,
                  RegistersT& registers,
                  Instruction instruction)
{
  uint16_t reg_index = instruction.destination[0];
  bool is_byte = (instruction.dest_type == DestinationType::Byte);
  Instructions::Mode mode = instruction.mode[0];
  StatusRegister state(registers[Registers::status]);

  uint16_t value = 0;
  uint16_t lsb = 0;
  uint16_t bitmask = 0b1000'0000;
  uint16_t target_address = 0;

  value = get_operand_value(memory, registers, reg_index, mode, is_byte);
  if (is_byte) {
    bitmask = 0b1000'0000'0000'0000;
  }
  lsb = value & 0b1;
  value >>= 1;
  if (state.get_carry()) {
    value |= bitmask;
  }
  set_operand_value(value, memory, registers, reg_index, mode, is_byte);

  if (lsb) {
    state.set_carry();
  } else {
    state.unset_carry();
  }
  set_status(state, value, is_byte);
}