#include "Cpu.hpp"

#include <iostream>

Cpu::Cpu(Bus *bus) : m_bus(bus) {}

void Cpu::log() const {
  std::cout << "Program counter   : " << std::hex << (int)m_pc << "\n";
  std::cout << "Accumulator       : " << std::hex << (int)m_a << "\n";
  std::cout << "Effective address : " << std::hex << (int)m_effective_address
            << "\n";

  std::cout << "Flags             : N V X B D I Z C\n";
  std::cout << "                    " << (int)m_p.negative << " "
            << (int)m_p.overflow << " " << (int)m_p.expansion << " "
            << (int)m_p.break_command << " " << (int)m_p.decimal_mode << " "
            << (int)m_p.interrupt_disable << " " << (int)m_p.zero << " "
            << (int)m_p.carry << "\n";
}

void Cpu::test() {
  Bus bus;
  Cpu cpu(&bus);
  std::cout << std::hex << cpu.m_p << std::endl;
}

uint8_t Cpu::immediate_addresing() {
  m_effective_address = m_pc++;
  return 0;
}

uint8_t Cpu::absolute_addresing() {
  // read low order byte.
  m_effective_address = m_bus->read(m_pc++);
  // read high order byte.
  m_effective_address =
      ((uint16_t)m_bus->read(m_pc++) << 8) | m_effective_address;
  return 0;
}

uint8_t Cpu::zero_page_addressing() {
  m_effective_address = m_bus->read(m_pc++);
  return 0;
}

uint8_t Cpu::absolute_x_indexed() {
  // read low order byte.
  m_effective_address = m_bus->read(m_pc++);
  // read high order byte.
  m_effective_address =
      ((uint16_t)m_bus->read(m_pc++) << 8) | m_effective_address;

  m_effective_address += m_x;
  return 0;
}

uint8_t Cpu::absolute_y_indexed() {
  // read low order byte.
  m_effective_address = m_bus->read(m_pc++);
  // read high order byte.
  m_effective_address =
      ((uint16_t)m_bus->read(m_pc++) << 8) | m_effective_address;

  m_effective_address += m_y;
  return 0;
}

uint8_t Cpu::zero_page_x_indexed() {
  m_effective_address = (m_bus->read(m_pc++) + m_x) % 256;
  return 0;
}

uint8_t Cpu::zero_page_y_indexed() {
  m_effective_address = (m_bus->read(m_pc++) + m_y) % 256;
  return 0;
}

uint8_t Cpu::indexed_indirect() {
  // val = PEEK(PEEK((arg + X) % 256) + PEEK((arg + X + 1) % 256) * 256)
  m_effective_address = m_bus->read(m_pc++);
  m_effective_address =
      m_bus->read((m_effective_address + m_x) % 256) +
      m_bus->read((m_effective_address + m_x + 1) % 256) * 256;
  return 0;
}

uint8_t Cpu::indirect_indexed() {
  // val = PEEK(PEEK(arg) + PEEK((arg + 1) % 256) * 256 + Y)
  m_effective_address = m_bus->read(m_pc++);
  m_effective_address = m_bus->read(m_effective_address) +
                        m_bus->read((m_effective_address + 1) % 256) * 256 +
                        m_y;
  return 0;
}

uint8_t Cpu::AND() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_a &= m_fetched_data;
  m_p.zero = (m_a == 0);
  m_p.negative = (m_a >> 7);
  return 0;
}

uint8_t Cpu::ORA() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_a |= m_fetched_data;
  m_p.zero = (m_a == 0);
  m_p.negative = (m_a >> 7);
  return 0;
}

uint8_t Cpu::EOR() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_a ^= m_fetched_data;
  m_p.zero = (m_a == 0);
  m_p.negative = (m_a >> 7);
  return 0;
}

uint8_t Cpu::ADC() {
  uint16_t result = 0;
  m_fetched_data = m_bus->read(m_effective_address);
  result = m_a + m_fetched_data + m_p.carry;
  /// set appropriate flags.
  m_p.carry = result > 255;
  m_p.zero = !(result & 0xFF);
  m_p.negative = (result & 0x80) > 0;

  if ((m_a & 0x80) == (m_fetched_data & 0x80)) {
    // if both the numbers have same sign then their is possibility of overflow.
    // check the sign bit of result and accumulator before addition.
    // if sign is changed then set overflow flag else reset.
    m_p.overflow = (m_a & 0x80) != (result & 0x80);
  }
  // store result in accumulator.
  m_a = result & 0xFF;
  return 0;
}

uint8_t Cpu::SBC() {
  uint16_t result = 0;
  m_fetched_data = m_bus->read(m_effective_address);
  result = m_a - m_fetched_data - m_p.carry;

  m_p.carry = (result & 0x80) == 0;
  m_p.zero = (result & 0xFF) == 0;
  m_p.negative = (result & 0x80) > 0;

  if ((m_a & 0x80) != (m_fetched_data & 0x80)) {
    // if both the numbers have different sign then their is possibility of
    // overflow. check the sign bit of result and accumulator before addition.
    // if sign is changed then set overflow flag else reset.
    m_p.overflow = (m_a & 0x80) != (result & 0x80);
  }
  // store result in accumulator.
  m_a = result & 0xFF;
  return 0;
}

uint8_t Cpu::CMP() {
  uint16_t result = 0;
  m_fetched_data = m_bus->read(m_effective_address);
  result = m_a - m_fetched_data;

  m_p.zero = (result & 0xFF) == 0;
  m_p.negative = (result & 0x80) > 1;
  m_p.carry = (m_fetched_data <= m_a);
  return 0;
}

uint8_t Cpu::CPX() {
  uint16_t result = 0;
  m_fetched_data = m_bus->read(m_effective_address);
  result = m_x - m_fetched_data;

  m_p.zero = (result & 0xFF) == 0;
  m_p.negative = (result & 0x80) > 1;
  m_p.carry = (m_fetched_data <= m_x);
  return 0;
}

uint8_t Cpu::CPY() {
  uint16_t result = 0;
  m_fetched_data = m_bus->read(m_effective_address);
  result = m_y - m_fetched_data;

  m_p.zero = (result & 0xFF) == 0;
  m_p.negative = (result & 0x80) > 1;
  m_p.carry = (m_fetched_data <= m_y);
  return 0;
}

uint8_t Cpu::DEC() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_fetched_data--;
  m_bus->write(m_effective_address, m_fetched_data);

  m_p.zero = (m_fetched_data == 0);
  m_p.negative = (m_fetched_data & 0x80) > 0;
  return 0;
}

uint8_t Cpu::DEX() {
  m_x--;

  m_p.zero = (m_x == 0);
  m_p.negative = (m_x & 0x80) > 0;
  return 0;
}

uint8_t Cpu::DEY() {
  m_y--;

  m_p.zero = (m_y == 0);
  m_p.negative = (m_y & 0x80) > 0;
  return 0;
}

uint8_t Cpu::INC() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_fetched_data++;
  m_bus->write(m_effective_address, m_fetched_data);

  m_p.zero = (m_fetched_data == 0);
  m_p.negative = (m_fetched_data & 0x80) > 0;
  return 0;
}

uint8_t Cpu::INX() {
  m_x++;

  m_p.zero = (m_x == 0);
  m_p.negative = (m_x & 0x80) > 0;
  return 0;
}

uint8_t Cpu::INY() {
  m_y++;

  m_p.zero = (m_y == 0);
  m_p.negative = (m_y & 0x80) > 0;
  return 0;
}

uint8_t Cpu::ASL() {
  // TODO: implement for implied addressing mode.
  m_fetched_data = m_bus->read(m_effective_address);
  m_p.carry = (m_fetched_data & 0x80) > 0;
  m_fetched_data = m_fetched_data << 1;
  m_p.negative = (m_fetched_data & 0x80) > 0;
  m_p.zero = (m_fetched_data == 0);
  m_bus->write(m_effective_address, m_fetched_data);
  return 0;
}

uint8_t Cpu::ROL() {
  m_fetched_data = m_bus->read(m_effective_address);
  uint8_t result = m_fetched_data << 1;
  result = result | m_p.carry;
  m_p.carry = (m_fetched_data & 0x80) > 0;
  m_bus->write(m_effective_address, result);
  return 0;
}

uint8_t Cpu::LSR() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_p.carry = m_fetched_data & 1;
  m_fetched_data = m_fetched_data >> 1;
  m_bus->write(m_effective_address, m_fetched_data);
  return 0;
}

uint8_t Cpu::ROR() {
  m_fetched_data = m_bus->read(m_effective_address);
  uint8_t result = m_fetched_data >> 1;
  result = result | (m_p.carry << 7);
  m_p.carry = m_fetched_data & 1;
  m_bus->write(m_effective_address, result);
  return 0;
}

uint8_t Cpu::LDA() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_a = m_fetched_data;
  m_p.zero = (m_a == 0);
  m_p.negative = (m_a & 0x80) > 0;
  return 0;
}

uint8_t Cpu::STA() {
  m_bus->write(m_effective_address, m_a);
  return 0;
}

uint8_t Cpu::LDX() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_x = m_fetched_data;
  m_p.zero = (m_x == 0);
  m_p.negative = (m_x & 0x80) > 0;
  return 0;
}

uint8_t Cpu::STX() {
  m_bus->write(m_effective_address, m_x);
  return 0;
}

uint8_t Cpu::LDY() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_y = m_fetched_data;
  m_p.zero = (m_y == 0);
  m_p.negative = (m_y & 0x80) > 0;
  return 0;
}

uint8_t Cpu::STY() {
  m_bus->write(m_effective_address, m_y);
  return 0;
}

uint8_t Cpu::TAX() {
  m_x = m_a;
  m_p.zero = (m_x == 0);
  m_p.negative = (m_x & 0x80) > 0;
  return 0;
}

uint8_t Cpu::TXA() {
  m_a = m_x;
  m_p.zero = (m_a == 0);
  m_p.negative = (m_a & 0x80) > 0;
  return 0;
}

uint8_t Cpu::TAY() {
  m_y = m_a;
  m_p.zero = (m_y == 0);
  m_p.negative = (m_y & 0x80) > 0;
  return 0;
}

uint8_t Cpu::TYA() {
  m_a = m_y;
  m_p.zero = (m_a == 0);
  m_p.negative = (m_a & 0x80) > 0;
  return 0;
}

uint8_t Cpu::TSX() {
  m_x = m_s;
  m_p.zero = (m_x == 0);
  m_p.negative = (m_x & 0x80) > 0;
  return 0;
}

uint8_t Cpu::TXS() {
  m_s = m_x;
  m_p.zero = (m_s == 0);
  m_p.negative = (m_s & 0x80) > 0;
  return 0;
}

uint8_t Cpu::PLA() {
  m_s++;
  m_a = m_bus->read(m_s);
  m_p.zero = (m_a == 0);
  m_p.negative = (m_a & 0x80) > 0;
  return 0;
}

uint8_t Cpu::PHA() {
  m_s--;
  m_bus->write(m_s, m_a);
  return 0;
}

uint8_t Cpu::PLP() { return 0; }
