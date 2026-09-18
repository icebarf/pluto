#pragma once
#ifndef ICE_PDP_UTILITIES_H
#define ICE_PDP_UTILITIES_H

#include "types.hpp"

uint16_t
read_word(const Types::Memory_t& memory, const uint16_t address);

void
write_word(Types::Memory_t& memory,
           const uint16_t address,
           const uint16_t value);

#endif