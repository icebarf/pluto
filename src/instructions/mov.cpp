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
#include <cstdint>

using namespace Instructions;
using namespace Types;

static void
set_status(StatusRegister status, uint16_t value, bool is_byte)
{
  status.set_negative_zero(value, is_byte);
  status.unset_overflow();
}

void
Instructions::mov(Types::MemoryT& memory,
                  Types::RegistersT& registers,
                  Instruction instruction)
{
  DoubleOperand operands =
    fetch_double_operands(memory, registers, instruction, false);
  uint16_t value = operands.source_value;
  if (operands.is_byte && operands.destination.is_register) {
    value = static_cast<int16_t>(static_cast<int8_t>(
      value & 0x00ff)); // explicitly cast value to signed 8 bit -> 16-bit ->
                        //  implicit unsigned 16-bit, giving us a sign extension
                        //  in first two steps

    // manually assign instead of using `write_operand` (the function
    // preserves higher 16-bits for a register and byte value)
    registers[operands.destination.reg] = value;
  } else {
    write_operand(memory,
                  registers,
                  operands.destination,
                  operands.source_value,
                  operands.is_byte);
  }

  set_status(StatusRegister(registers[Registers::status]),
             operands.source_value,
             operands.is_byte);
}