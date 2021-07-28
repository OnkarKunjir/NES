#pragma once

#include "Bus.hpp"

#include <cstdint>

/**
 * Structure to hold various flags used in processor status word in 6502.
 */
struct Flag {
  uint8_t carry : 1;
  uint8_t zero : 1;
  uint8_t interrupt_disable : 1;
  uint8_t decimal_mode : 1;
  uint8_t break_command : 1;
  uint8_t expansion : 1;
  uint8_t overflow : 1;
  uint8_t negative : 1;
};

/**
 * Cpu class emulates behaviour of 6502 processor used in NES.
 */
class Cpu {
public:
  Cpu(Bus *bus);

  void log() const;

  static void test();

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

  /// Processor status register.
  Flag m_p;

  /// holds effective address of the data, simulates behaviour of ADH and ADL.
  uint16_t m_effective_address;

  /// holds fetched data from the address m_absolute_address.
  uint8_t m_fetched_data;

  /**
   * Immidiate addressing uses opcode as value.
   * Each instruction is 2 bytes instruction containing opcode and oprand.
   */
  uint8_t immediate_addresing();

  /**
   * Absolute addressing is 3 byte instruction. Contains 2 byte address of data.
   * opcode | low address byte | high address byte
   */
  uint8_t absolute_addresing();

  /**
   * Zero page addressing is 2 byte instruction. Opcode is used as index in page
   * 0 of memory.
   */
  uint8_t zero_page_addressing();

  /**
   * Used in braching instructions.
   * TODO: yet to be implemented.
   */
  uint8_t relative_addressing();

  /**
   * Similar to absolute_addressing but X index is added to obtain effective
   * address.
   */
  uint8_t absolute_x_indexed();

  /**
   * Similar to absolute_addressing but Y index is added to obtain effective
   * address.
   */
  uint8_t absolute_y_indexed();

  /**
   * Similar to zero_page_addressing but X index is added to obtain effective
   * address. If effective address > 0xFF then effective_address =
   * effective_address % 256
   */
  uint8_t zero_page_x_indexed();

  /**
   * Similar to zero_page_addressing but Y index is added to obtain effective
   * address. If effective address > 0xFF then effective_address =
   * effective_address % 256
   */
  uint8_t zero_page_y_indexed();

  /**
   * Indexed indirect is 2 byte long instruction and uses X index register to
   * calculate effective address.
   * @see http://users.telenet.be/kim1-6502/6502/proman.html#f65
   */
  uint8_t indexed_indirect();

  /**
   * Indirect indexed is 2 byte long insturction and uses Y index register to
   * calculate effective address.
   * @see http://users.telenet.be/kim1-6502/6502/proman.html#f66
   */
  uint8_t indirect_indexed();

  /**
   * Bitwise AND the memory contents with the accumulator and store result in
   * the accumulator.
   * Set zero flag if accumulator is zero.
   * Set negative flag if 7th bit of accumulator is set.
   */
  uint8_t AND();

  /**
   * Bitwise OR the memory contents with the accumulator and store result in
   * the accumulator.
   * Set zero flag if accumulator is zero.
   * Set negative flag if 7th bit of accumulator is set.
   */
  uint8_t ORA();

  /**
   * Bitwise EXOR the memory contents with the accumulator and store result in
   * the accumulator.
   * Set zero flag if accumulator is zero.
   * Set negative flag if 7th bit of accumulator is set.
   */
  uint8_t EOR();

  /**
   * Add memory to accumulator with carry and store result in accumulator.
   * Set zero flag if accumulator is zero.
   * Set carry flag if result is > 255.
   * Set negative flag if 7th bit of accumulator is set.
   * Set overflow flag if addition changes the sign of result in signed additon.
   */
  uint8_t ADC();

  /**
   * Substract memory from accumulator with carry and store result in
   * accumulator.
   * Set zero flag if accumulator is zero.
   * Set carry flag if result is >= 0.
   * Set negative flag if 7th bit of accumulator is set.
   * Set overflow flag if substraction changes the sign of result in signed
   * additon.
   */
  uint8_t SBC();

  /**
   * Compare contents of accumulator and memory by substracting value from
   * accumulator.
   * Set zero flag if both are equal.
   * Set carry flag if value is less than or equal to accumulator.
   * Set negative flag if result of substaction is negative.
   */
  uint8_t CMP();

  /**
   * Compare contents of X index register and memory by substracting value from
   * it.
   * Set zero flag if both are equal.
   * Set carry flag if value is less than or equal to accumulator.
   * Set negative flag if result of substaction is negative.
   */
  uint8_t CPX();

  /**
   * Compare contents of X index register and memory by substracting value from
   * it.
   * Set zero flag if both are equal.
   * Set carry flag if value is less than or equal to accumulator.
   * Set negative flag if result of substaction is negative.
   */
  uint8_t CPY();

  /**
   * Decrement value of memory location by one.
   * Set negative flag if 7th bit is set for result.
   * Set zero flag if reuslt is zero.
   */
  uint8_t DEC();

  /**
   * Decrement value of index register X and store it in index register X.
   * Set negative flag if 7th bit is set.
   * Set zero flag if index register X is zero.
   */
  uint8_t DEX();

  /**
   * Decrement value of index register Y and store it in index register Y.
   * Set negative flag if 7th bit is set.
   * Set zero flag if index register Y is zero.
   */
  uint8_t DEY();

  /**
   * Increment value of memory location by one.
   * Set negative flag if 7th bit is set.
   * Set zero flag if value is equal to zero after operation.
   */
  uint8_t INC();

  /**
   * Increment value of X index register by one.
   * Set negative flag if 7th bit is set.
   * Set zero flag if value is equal to zero after operation.
   */
  uint8_t INX();

  /**
   * Increment value of X index location by one.
   * Set negative flag if 7th bit is set.
   * Set zero flag if value is equal to zero after operation.
   */
  uint8_t INY();

  /**
   * Shift the contnet of accumulator or memory location to left by one bit. 7th
   * bit is stored in carry flag.
   *
   */
  // TODO: Change behaviour depending upon addressing mode.
  uint8_t ASL();

  /**
   * Rotate content of accumulator or memory to left.
   */
  uint8_t ROL();

  /**
   * Shit content of accumulator or memory to right.
   */
  uint8_t LSR();

  /**
   * Rotate content of accumulator or memory to right.
   */
  uint8_t ROR();
};
