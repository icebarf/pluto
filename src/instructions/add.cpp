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
#include <cassert>
#include <cstdint>

using namespace Instructions;
using namespace Types;

static void
set_status(StatusRegister status,
           uint32_t value,
           int16_t source,
           int16_t destination,
           bool is_byte)
{
  status.set_negative_zero(static_cast<uint16_t>(value), is_byte);
  bool carry = value >> 16 & 1;
  carry ? status.set_carry() : status.unset_carry();

  bool result_sign = !status.get_negative();
  bool source_sign = source > 0;
  bool destination_sign = destination > 0;

  // complement of xor of src ^ dst because we are going by the manual:
  // if signs of operands agree and sign of an operand and result disagree
  // then set V to 1 else set V to 0
  if ((source_sign == destination_sign) && (source_sign != result_sign)) {
    status.set_overflow();
  } else {
    status.unset_overflow();
  }
}

void
Instructions::add(Types::MemoryT& memory,
                  Types::RegistersT& registers,
                  Instruction instruction)
{
  DoubleOperand operands =
    fetch_double_operands(memory, registers, instruction, true);
  assert(operands.is_byte == false);
  uint16_t source = operands.source_value;
  uint16_t destination = operands.destination_value;

  uint32_t sum = destination + source;

  write_operand(
    memory, registers, operands.destination, static_cast<uint16_t>(sum), false);

  set_status(StatusRegister(registers[Registers::status]),
             sum,
             source,
             destination,
             false);
}