#pragma once

#include "Bus.hpp"

#include <cstdint>

/**
 * Cpu class emulates behaviour of 6502 processor used in NES.
 */
class Cpu {
public:
  Cpu(Bus *bus);

  /// Function to test addressing modes.
  static void test_address_modes();

private:
  /// Context of bus.
  Bus *m_bus;

  /// Accumularot
  uint8_t m_a;

  /// Index registers.
  uint8_t m_x, m_y;

  /// Stack pointer.
  uint8_t m_s;

  /// Program counter.
  uint16_t m_pc;

  /// holds effective address of the data, simulates behaviour of ADH and ADL.
  uint16_t m_effective_address;

  /// holds fetched data from the address m_absolute_address.
  uint8_t m_fetched_data;

  uint8_t immediate_addresing();
  uint8_t absolute_addresing();
  uint8_t zero_page_addressing();
  uint8_t relative_addressing();
  uint8_t absolute_x_indexed();
  uint8_t absolute_y_indexed();
  uint8_t zero_page_x_indexed();
  uint8_t zero_page_y_indexed();
  uint8_t indexed_indirect();
  uint8_t indirect_indexed();
};
