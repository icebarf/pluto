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