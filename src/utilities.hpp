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
#pragma once
#ifndef ICE_PDP_UTILITIES_H
#define ICE_PDP_UTILITIES_H

#include "types.hpp"

constexpr int signed_byte_max_octal = 0200;
constexpr int signed_word_max_octal = 0100000;

uint16_t
read_word(const Types::MemoryT& memory, const uint16_t address);

void
write_word(Types::MemoryT& memory,
           const uint16_t address,
           const uint16_t value);
uint8_t
read_byte(const Types::MemoryT& memory, const uint16_t address);

void
write_byte(Types::MemoryT& memory,
           const uint16_t address,
           const uint16_t value);
#endif