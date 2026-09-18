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

using namespace Instruction;

// we have word
// bit 15: 0/1 (1= byte Destination and not the full word)
// bit 14-12: 1 digit octal
// bit 11-9: 2 digit
// bit 8-6: 3 digit
// bit 5-3: 4 digit
// bit 2-0: 5 digit

// if bit 14-12 is in range: [1,6] then it is a double operand instruction
// otherwise single operand

// Refer PDP11/20 Handbook pdf chapter 3.1 and 3.2 for instruction layouts for
// single operand addressing and double operand addressing

inline instruction
extract_single_operand_instruction(uint16_t word)
{
  return instruction{ .opcode = static_cast<uint16_t>((word >> 6)),
                      .destination = { static_cast<Registers>(word & 0x07) },
                      .mode = { static_cast<Mode>((word >> 3) & 0x7) } };
};

inline instruction
extract_double_operand_instruction(uint16_t word)
{
  return instruction{ .opcode = static_cast<uint16_t>((word >> 12) & 0x7),
                      .destination = { static_cast<Registers>(word & 0x07),
                                       static_cast<Registers>((word >> 6) &
                                                              0x7) },
                      .mode = { static_cast<Mode>((word >> 3) & 0x7),
                                static_cast<Mode>((word >> 9) & 0x7) } };
};

instruction
extract(uint16_t word)
{
  uint8_t digit_1 =
    (word >> 12) &
    0b0000'0111; // we also turn off the bit for byte/word detection

  DestinationType type = [word]() {
    if (word >> 15)
      return DestinationType::Byte;
    else
      return DestinationType::Word;
  }();

  bool double_operand = false;

  if (digit_1 >= 1 && digit_1 <= 6) {
    double_operand = true;
  }

  instruction instruction;
  if (double_operand) {
    instruction = extract_double_operand_instruction(word);
  } else {
    instruction = extract_single_operand_instruction(word);
  }

  instruction.dest_type = type;
  return instruction;
}