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
#ifndef ICE_PDP_TYPES_H
#define ICE_PDP_TYPES_H

#include <array>
#include <cstdint>

namespace Types {

enum Registers
{
  r0 = 0,
  r1 = 1,
  r2 = 2,
  r3 = 3,
  r4 = 4,
  r5 = 5,
  r6 = 6,
  r7 = 7,
  stack_pointer = 6,
  program_counter = 7,
  status = 8
};

using Registers_t = std::array<uint16_t, 9>;
using Memory_t = std::array<uint8_t, 56 * 1024>;
} // namespace Types ;

#endif