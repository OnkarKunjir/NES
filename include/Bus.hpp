#pragma once

#include <cstdint>

/**
 * Class emulates behaviour of the bus.
 *
 * Bus can be used to transfer 1 byte data from 2 byte addressable memory.
 */
class Bus {
public:
  Bus();
  ~Bus();

  /// Read 1 byte of data from address.
  uint8_t read(uint16_t address) const;

  /// Write 1 byte of data from address.
  void write(uint16_t address, uint8_t data);

private:
  uint8_t m_ram[0xffff];
};
