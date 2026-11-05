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
set_status(StatusRegister state, uint16_t value, bool is_byte)
{
  state.set_negative_zero(value, is_byte);
  state.unset_overflow();
}

void
Instructions::bis(Types::MemoryT& memory,
                  Types::RegistersT& registers,
                  Instruction instruction)
{
  DoubleOperand operands =
    fetch_double_operands(memory, registers, instruction, true);
  uint16_t result = operands.source_value | operands.destination_value;
  write_operand(
    memory, registers, operands.destination, result, operands.is_byte);

  set_status(
    StatusRegister(registers[Registers::status]), result, operands.is_byte);
}