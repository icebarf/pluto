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
#include "utilities.hpp"
#include <cstdint>

uint16_t
read_word(const Types::MemoryT& memory, const uint16_t address)
{
  // add trap check here for unaligned odd accesses.
  // low bytes are stored at even address  and  high bytes are at odd
  return memory[address] | (memory[address + 1] << 8);
}

void
write_word(Types::MemoryT& memory, const uint16_t address, const uint16_t value)
{
  memory[address] = value & 0xff;
  memory[address + 1] = (value >> 8) & 0xff;
}

uint8_t
read_byte(const Types::MemoryT& memory, const uint16_t address)
{
  return memory[address];
}

void
write_byte(Types::MemoryT& memory, const uint16_t address, const uint16_t value)
{
  memory[address] = value;
}