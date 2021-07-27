#include "Cpu.hpp"

#include <iostream>

Cpu::Cpu(Bus *bus) : m_bus(bus) {}

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

uint8_t Cpu::OR() {
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
  m_fetched_data = m_bus->read(m_effective_address);
  uint16_t result = m_a + m_fetched_data + m_p.carry;
  m_p.carry = result > 0xff;
  m_p.negative = (result & 1 << 7) > 0;
  if ((m_a & (1 << 7)) == (m_fetched_data & (1 << 7)))
    m_p.overflow = (result & (1 << 7)) != (m_a & (1 << 7));
  m_a = result;
  m_p.zero = (m_a == 0);
  return 0;
}

uint8_t Cpu::SBC() {
  m_fetched_data = m_bus->read(m_effective_address);
  uint16_t result = m_a - m_fetched_data - m_p.carry;

  m_p.carry = (result & (1 << 7)) > 0;
  m_p.negative = (result & (1 << 7)) > 0;

  if ((m_a & (1 << 7)) == (m_fetched_data & (1 << 7)))
    m_p.overflow = (result & (1 << 7)) != (m_a & (1 << 7));

  return 0;
}
