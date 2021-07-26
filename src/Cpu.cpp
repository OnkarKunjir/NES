#include "Cpu.hpp"

#include <iostream>

Cpu::Cpu(Bus *bus) : m_bus(bus) {}

void Cpu::test_address_modes() {
  Bus bus;
  // place holder data.
  for (int i = 0; i <= 0xff; i++)
    bus.write(i, i);

  Cpu cpu(&bus);

  std::cout << "Immidiate addresing : ";
  cpu.m_pc = 0xfc;
  cpu.immediate_addresing();
  if (cpu.m_pc == 0xfd && cpu.m_effective_address == 0xfc)
    std::cout << "Passed\n";
  else
    std::cout << "Failed\n";

  std::cout << "Absolute addresing : ";
  cpu.m_pc = 0xf1;
  cpu.absolute_addresing();
  if (cpu.m_pc == 0xf3 && cpu.m_effective_address == 0xf2f1)
    std::cout << "Passed\n";
  else
    std::cout << "Failed\n";

  std::cout << "Zero page addresing : ";
  bus.write(0xb0c0, 0x55);
  cpu.m_pc = 0xb0c0;
  cpu.zero_page_addressing();
  if (cpu.m_pc == 0xb0c1 && cpu.m_effective_address == 0x0055)
    std::cout << "Passed\n";
  else
    std::cout << "Failed\n";

  std::cout << "Absolute x indexed : ";
  cpu.m_pc = 0x20;
  cpu.m_x = 0xff;
  cpu.absolute_x_indexed();
  if (cpu.m_pc == 0x22 && cpu.m_effective_address == (0x2120 + 0xff))
    std::cout << "Passed\n";
  else
    std::cout << "Failed\n";

  std::cout << "Absolute y indexed : ";
  cpu.m_pc = 0x10;
  cpu.m_y = 0xff;
  cpu.absolute_x_indexed();
  if (cpu.m_pc == 0x12 && cpu.m_effective_address == (0x1110 + 0xff))
    std::cout << "Passed\n";
  else
    std::cout << "Failed\n";

  std::cout << "Zero page x indexed : ";
  bus.write(0xc0c0, 0x02);
  cpu.m_pc = 0xc0c0;
  cpu.m_x = 0x2f;
  cpu.zero_page_x_indexed();
  if (cpu.m_pc == 0xc0c1 && cpu.m_effective_address == (0x0002 + 0x2f))
    std::cout << "Passed\n";
  else
    std::cout << "Failed\n";

  std::cout << "Zero page y indexed : ";
  bus.write(0xc0c0, 0x02);
  cpu.m_pc = 0xc0c0;
  cpu.m_y = 0x2f;
  cpu.zero_page_y_indexed();
  if (cpu.m_pc == 0xc0c1 && cpu.m_effective_address == (0x0002 + 0x2f))
    std::cout << "Passed\n";
  else
    std::cout << "Failed\n";

  std::cout << "Indexed indirect : ";
  bus.write(0x3030, 0x20);
  cpu.m_pc = 0x3030;
  cpu.m_x = 0x04;
  cpu.indexed_indirect();
  if (cpu.m_pc == 0x3031 && cpu.m_effective_address == 0x2524)
    std::cout << "Passed\n";
  else
    std::cout << "Failed\n";

  std::cout << "Indirect indexed : ";
  bus.write(0x3030, 0x20);
  cpu.m_pc = 0x3030;
  cpu.m_y = 0x0f;
  cpu.indirect_indexed();
  if (cpu.m_pc == 0x3031 && cpu.m_effective_address == 0x212f)
    std::cout << "Passed\n";
  else
    std::cout << "Failed\n";
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
