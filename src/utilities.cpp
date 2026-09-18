#include "utilities.hpp"

uint16_t
read_word(const Types::Memory_t& memory, const uint16_t address)
{
  // add trap check here for unaligned odd accesses.
  // low bytes are stored at even address  and  high bytes are at odd
  return memory[address] | (memory[address + 1] << 8);
}

void
write_word(Types::Memory_t& memory,
           const uint16_t address,
           const uint16_t value)
{
  memory[address] = value & 0xff;
  memory[address + 1] = (value >> 8) & 0xff;
}