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
