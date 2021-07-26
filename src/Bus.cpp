#include "Bus.hpp"

Bus::Bus() {}
Bus::~Bus() {}

uint8_t Bus::read(uint16_t address) const {
  if (address <= 0xffff)
    return m_ram[address];
  return 0;
}
void Bus::write(uint16_t address, uint8_t data) {
  if (address <= 0xffff)
    m_ram[address] = data;
}
