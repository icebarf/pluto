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
  this->status_register = this->status_register | (0xffff & status);
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

void
StatusRegister::set_negative_zero(uint16_t value, bool is_byte)
{
  uint8_t byte_value = value & 0xff;
  if (is_byte) {
    byte_value == 0 ? this->set_zero() : this->unset_zero();
    (byte_value >> 7) & 0b1 ? this->set_negative() : this->unset_negative();
    return;
  }

  value == 0 ? this->set_zero() : this->unset_zero();
  (value >> 15) & 0b1 ? this->set_negative() : this->unset_negative();
}

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
Instruction
Instructions::extract(const uint16_t word)
{
  Instruction instruction{};
  const uint16_t group = word & 0x7000;

  if (group == 0) { // single operand
    instruction.is_double = false;
    instruction.opcode = (word >> 6) & 0x01ff;
    instruction.destination[0] = static_cast<Registers>(word & 0x07);
    instruction.mode[0] = static_cast<Mode>((word >> 3) & 0x07);
    instruction.dest_type =
      (word & 0x8000) ? DestinationType::Byte : DestinationType::Word;

  } else if (group <= 060000) { // double operand, 01..06 / 16
    instruction.is_double = true;
    instruction.opcode = word >> 12; // 001..006, 011..015, 016
    instruction.destination[0] = static_cast<Registers>(word & 0x07);
    instruction.mode[0] = static_cast<Mode>((word >> 3) & 0x07);
    instruction.destination[1] = static_cast<Registers>((word >> 6) & 0x07);
    instruction.mode[1] = static_cast<Mode>((word >> 9) & 0x07);

    bool byte_op = instruction.opcode >= 011 && instruction.opcode <= 015;
    instruction.dest_type =
      byte_op ? DestinationType::Byte : DestinationType::Word;

  } else { // 07/17: EIS, illegal on 11/20
    instruction.is_double = false;
    instruction.opcode = word >> 12;
    instruction.dest_type = DestinationType::Word;
  }
  return instruction;
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
    // SP
    case RegisterDeferred: {
      address = registers[reg];
    } break;

    // also Immediate (PC)
    case AutoIncrement: {
      address = registers[reg];
      registers[reg] +=
        (!is_byte || reg >= 6) ? 2 : 1; // handles 6 = SP, 7 = PC
    } break;
    // also Aboslute (PC) and for SP
    case AutoIncrementDeferred: {
      address = read_word(memory, registers[reg]);
      registers[reg] += 2;
    } break;

    // General Registers (incl. SP & PC)
    case AutoDecrement: {
      registers[reg] -=
        (!is_byte || reg >= 6) ? 2 : 1; // handles 6 = SP, 7 = PC
      address = registers[reg];
    } break;

    // for SP
    case AutoDecrementDeferred: {
      registers[reg] -= 2;
      address = read_word(memory, registers[reg]);
    } break;

    // also Relative (PC)
    case Index: {
      // offset exists in the word following the current instruction
      uint16_t offset =
        read_word(memory, registers[Registers::program_counter]);
      // move the PC past the offset/index value
      registers[Registers::program_counter] += 2;
      address = registers[reg] + offset;
    } break;

    // also RelativeDeferred (PC) and for SP
    case IndexDeferred: {
      uint16_t offset =
        read_word(memory, registers[Registers::program_counter]);
      registers[Registers::program_counter] += 2;
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
      registers[reg_index] = (registers[reg_index] & 0xff00) | (value & 0x00ff);
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