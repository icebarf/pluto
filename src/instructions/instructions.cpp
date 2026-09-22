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
#include "../utilities.hpp"
#include <cstdint>

using namespace Instructions;

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

inline Instruction
extract_single_operand_instruction(uint16_t word)
{
  return Instruction{ .opcode = static_cast<uint16_t>((word >> 6)),
                      .destination = { static_cast<Registers>(word & 0x07) },
                      .mode = { static_cast<Mode>((word >> 3) & 0x7) } };
};

inline Instruction
extract_double_operand_instruction(uint16_t word)
{
  return Instruction{ .opcode = static_cast<uint16_t>((word >> 12) & 0x7),
                      .destination = { static_cast<Registers>(word & 0x07),
                                       static_cast<Registers>((word >> 6) &
                                                              0x7) },
                      .mode = { static_cast<Mode>((word >> 3) & 0x7),
                                static_cast<Mode>((word >> 9) & 0x7) } };
};

Instruction
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

  Instruction instruction;
  if (double_operand) {
    instruction = extract_double_operand_instruction(word);
  } else {
    instruction = extract_single_operand_instruction(word);
  }

  instruction.dest_type = type;
  return instruction;
}

constexpr uint16_t CARRY_SET = 0b0000'0000'0000'0001;
constexpr uint16_t CARRY_UNSET = 0b1111'1111'1111'1110;
constexpr uint16_t OVERFLOW_SET = 0b0000'0000'0000'0010;
constexpr uint16_t OVERFLOW_UNSET = 0b1111'1111'1111'1101;
constexpr uint16_t ZERO_SET = 0b0000'0000'0000'0100;
constexpr uint16_t ZERO_UNSET = 0b1111'1111'1111'1011;
constexpr uint16_t NEGATIVE_SET = 0b0000'0000'0000'1000;
constexpr uint16_t NEGATIVE_UNSET = 0b1111'1111'1111'0111;
constexpr uint16_t TRAP_SET = 0b0000'0000'0001'0000;
constexpr uint16_t TRAP_UNSET = 0b1111'1111'1110'1111;

void
set_status(StatusRegister::States bit, StatusRegister state, uint16_t& status)
{
  switch (bit) {
    case StatusRegister::Carry: {
      if (state.carry)
        status = status | CARRY_SET;
      else
        status = status & CARRY_UNSET;
    } break;

    case StatusRegister::Overflow: {
      if (state.overflow)
        status = status | OVERFLOW_SET;
      else
        status = status & OVERFLOW_UNSET;
    } break;

    case StatusRegister::Zero: {
      if (state.zero)
        status = status | ZERO_SET;
      else
        status = status & ZERO_UNSET;
    } break;

    case StatusRegister::Negative: {
      if (state.negative)
        status = status | NEGATIVE_SET;
      else
        status = status & NEGATIVE_UNSET;
    } break;

    case StatusRegister::Trap: {
      if (state.trap)
        status = status | TRAP_SET;
      else
        status = status & TRAP_UNSET;
    } break;

    case StatusRegister::Priority: {
      uint16_t tmp = 0xffff & (state.priority << 5);
      status = status & tmp;
    }
  }
}

void
set_status(StatusRegister state, uint16_t& status)
{
  set_status(StatusRegister::Carry, state, status);
  set_status(StatusRegister::Overflow, state, status);
  set_status(StatusRegister::Zero, state, status);
  set_status(StatusRegister::Negative, state, status);
  set_status(StatusRegister::Trap, state, status);
  set_status(StatusRegister::Priority, state, status);
}

uint16_t
get_operand_address(const Types::MemoryT& memory,
                    Types::RegistersT& registers,
                    Mode mode,
                    uint8_t reg,
                    bool is_byte)
{
  uint16_t address = 0;
  switch (mode) {
    case RegisterDeferred: {
      address = registers[reg];
    } break;

    case AutoIncrement: {
      address = registers[reg];
      registers[reg] += (!is_byte || reg >= 6) ? 2 : 1;
    } break;
    case AutoIncrementDeferred: {
      address = read_word(memory, registers[reg]);
      registers[reg] += 2;
    } break;

    case AutoDecrement: {
      registers[reg] -= (!is_byte || reg >= 6) ? 2 : 1;
      address = registers[reg];
    } break;
    case AutoDecrementDeferred: {
      registers[reg] -= 2;
      address = read_word(memory, registers[reg]);
    } break;

    case Index: {
      uint16_t offset =
        read_word(memory, registers[Types::Registers::program_counter]);
      registers[Types::Registers::program_counter] += 2;
      address = registers[reg] + offset;
    } break;

    case IndexDeferred: {
      uint16_t offset =
        read_word(memory, registers[Types::Registers::program_counter]);
      registers[Types::Registers::program_counter] += 2;
      address = read_word(memory, registers[reg] + offset);
    } break;

    case Register: {
      __builtin_unreachable();
    } break;
  }
  return address;
}