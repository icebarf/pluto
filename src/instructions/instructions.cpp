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
Instructions::extract(uint16_t word)
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

/* Status Register Bit positions for each status flag */
constexpr uint8_t carry_bit_pos = 0;
constexpr uint8_t overflow_bit_pos = 1;
constexpr uint8_t zero_bit_pos = 2;
constexpr uint8_t negative_bit_pos = 3;
constexpr uint8_t trap_bit_pos = 4;
constexpr uint8_t priority_bits_pos = 5;

/* Masks to set/unset each status flag */
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

StatusRegister::StatusRegister(uint16_t& status)
  : status_register(status)
{
}

void
StatusRegister::set_carry()
{
  status_register = status_register | CARRY_SET;
}
void
StatusRegister::unset_carry()
{
  status_register = status_register & CARRY_UNSET;
}

void
StatusRegister::set_overflow()
{
  status_register = status_register | OVERFLOW_SET;
}
void
StatusRegister::unset_overflow()
{
  status_register = status_register & OVERFLOW_UNSET;
}

void
StatusRegister::set_zero()
{
  status_register = status_register | ZERO_SET;
}
void
StatusRegister::unset_zero()
{
  status_register = status_register & ZERO_UNSET;
}

void
StatusRegister::set_negative()
{
  status_register = status_register | NEGATIVE_SET;
}
void
StatusRegister::unset_negative()
{
  status_register = status_register & NEGATIVE_UNSET;
}

void
StatusRegister::set_trap()
{
  status_register = status_register | TRAP_SET;
}
void
StatusRegister::unset_trap()
{
  status_register = status_register & TRAP_UNSET;
}

void
StatusRegister::set_priority(int value)
{
  uint16_t status =
    this->status_register & 0b0000'0000'0001'1111; // preserve status
  this->status_register = 0xffff & (value << 5);
  this->status_register = this->status_register & (0xffff & status);
}

bool
StatusRegister::get_carry() const
{
  return this->status_register & 0b1;
}

bool
StatusRegister::get_overflow() const
{
  return (this->status_register >> overflow_bit_pos) & 0b1;
}

bool
StatusRegister::get_zero() const
{
  return (this->status_register >> zero_bit_pos) & 0b1;
}

bool
StatusRegister::get_negative() const
{
  return (this->status_register >> negative_bit_pos) & 0b1;
}

bool
StatusRegister::get_trap() const
{
  return (this->status_register >> trap_bit_pos) & 0b1;
}

int
StatusRegister::get_priority() const
{
  return (this->status_register >> priority_bits_pos) & 0b111;
}

uint16_t
Instructions::get_operand_address(const Types::MemoryT& memory,
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

[[nodiscard]] uint16_t
Instructions::get_operand_value(Types::MemoryT& memory,
                                Types::RegistersT& registers,
                                uint8_t reg_index,
                                Instructions::Mode mode,
                                bool is_byte)
{
  uint16_t value;
  if (mode == Mode::Register) {
    if (is_byte) {
      value = registers[reg_index] & 0x00ff;
    } else {
      value = registers[reg_index];
    }
  } else {
    uint16_t target_address =
      get_operand_address(memory, registers, mode, reg_index, is_byte);
    if (is_byte) {
      value = read_byte(memory, target_address);
    } else {
      value = read_word(memory, target_address);
    }
  }
  return value;
}

void
Instructions::set_operand_value(uint16_t value,
                                Types::MemoryT& memory,
                                Types::RegistersT& registers,
                                uint8_t reg_index,
                                Instructions::Mode mode,
                                bool is_byte)
{
  if (mode == Mode::Register) {
    if (is_byte) {
      registers[reg_index] = (registers[reg_index] & 0xff00) | value;
    } else {
      registers[reg_index] = value;
    }

  } else {
    uint16_t target_address =
      get_operand_address(memory, registers, mode, reg_index, is_byte);
    if (is_byte) {
      write_byte(memory, target_address, value);
    } else {
      write_word(memory, target_address, value);
    }
  }
}