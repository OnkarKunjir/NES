#pragma once

#include "Bus.hpp"

#include <cstdint>
#include <string>
#include <vector>

/**
 * Cpu class emulates behaviour of 6502 processor used in NES.
 */
class Cpu {
public:
  Cpu(Bus *bus);

  void log() const;

  static void test();

  /**
   * Falg bit in processor status register.
   */
  enum Flag {
    carry = (1 << 0),
    zero = (1 << 1),
    interrupt_disable = (1 << 2),
    decimal_mode = (1 << 3),
    break_command = (1 << 4),
    expansion = (1 << 5),
    overflow = (1 << 6),
    negative = (1 << 7)
  };

  struct Instruction {
    std::string opcode;
    uint8_t (Cpu::*exec)(void) = nullptr;
    uint8_t (Cpu::*addressing)(void) = nullptr;
  };

  /**
   * Function executes instruction pointed by the program counter.
   */
  void tick();

private:
  /// Array contains mapping of intruction and addressing mode.
  std::vector<Instruction> m_lookup;

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
  uint8_t m_p;

  /// holds effective address of the data, simulates behaviour of ADH and ADL.
  uint16_t m_effective_address;

  /// holds fetched data from the address m_absolute_address.
  uint8_t m_fetched_data;

  /// holds the state of cpu.
  bool m_halt;

  /// holds the currently executing opcode.
  uint8_t m_opcode;

  /**
   * Function to set flag value to value in processor status register.
   * @param flag Specify flag to modify.
   * @param value Specify value to set as flag.
   */
  void set_flag(Flag flag, bool value);

  /**
   * @return Flag value speficied by flag.
   */
  uint8_t get_flag(Flag flag) const;

  /**
   * Push one byte of data onto the stack.
   */
  void push(uint8_t data);

  /**
   * Returns one byte of data at top of stack and updates the stack pointer.
   */
  uint8_t pop();

  uint8_t implicit_addressing();

  /**
   * Immidiate addressing uses opcode as value.
   * Each instruction is 2 bytes instruction containing opcode and oprand.
   */
  uint8_t immediate_addressing();

  /**
   * Absolute addressing is 3 byte instruction. Contains 2 byte address of data.
   * opcode | low address byte | high address byte
   */
  uint8_t absolute_addressing();

  /**
   * Zero page addressing is 2 byte instruction. Opcode is used as index in page
   * 0 of memory.
   */
  uint8_t zero_page_addressing();

  /**
   * Used in braching instructions.
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
   * Indirect indexing is 2 byte instruction where first byte is opcode and 2nd
   * byte is zero page index. Effective address is stored at zero page index.
   */
  uint8_t indirect_addressing();

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
   * Halts the CPU.
   * The data bus will be set to $FF
   */
  uint8_t KIL();

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

  /**
   * Load data from memory to accumulator.
   * Update flags accordingly.
   */
  uint8_t LDA();

  /**
   * Store accumulator into memory.
   */
  uint8_t STA();

  /**
   * Load index register X from memory location.
   */
  uint8_t LDX();

  /**
   * Store index register X into memory location.
   */
  uint8_t STX();

  /**
   * Load index register Y from memory location.
   */
  uint8_t LDY();

  /**
   * Store index register Y to memory location.
   */
  uint8_t STY();

  /**
   * Transfer accumulator to index register X.
   */
  uint8_t TAX();

  /**
   * Transfer index register X to accumulator.
   */
  uint8_t TXA();

  /**
   * Transfer accumulator to index register Y.
   */
  uint8_t TAY();

  /**
   * Transter index register Y to accumulator.
   */
  uint8_t TYA();

  /**
   * Transfer stack pointer to index register X.
   */
  uint8_t TSX();

  /**
   * Transter index register X to stack pointer.
   */
  uint8_t TXS();

  /**
   * Pull accumulator from stack.
   */
  uint8_t PLA();

  /**
   * Push accumulator onto the stack.
   */
  uint8_t PHA();

  /**
   * Pull status register from stack.
   */
  uint8_t PLP();

  /**
   * Push status register onto the stack.
   */
  uint8_t PHP();

  /**
   * Branch on result plus.
   */
  uint8_t BPL();

  /**
   * Branch on result minus.
   */
  uint8_t BMI();

  /**
   * Branch on overflow clear.
   */
  uint8_t BVC();

  /**
   * Branch on overflow set.
   */
  uint8_t BVS();

  /**
   * Branch on carry clear.
   */
  uint8_t BCC();

  /**
   * Branch on carry set.
   */
  uint8_t BCS();

  /**
   * Branch on result not zero.
   */
  uint8_t BNE();

  /**
   * Brach on result zero.
   */
  uint8_t BEQ();

  /**
   * Break current operation push processor status register and program counter
   * on stack and jump to inturrept subroutine at 0xFFFE/F. Insturction sets the
   * Break flag.
   */
  uint8_t BRK();

  /**
   * Return form inturrept.
   * Reads program counter and processor status register from stack.
   */
  uint8_t RTI();

  /**
   * Jump to subroutine.
   * Pushes current program counter onto the stack.
   */
  uint8_t JSR();

  /**
   * Return from subroutine.
   */
  uint8_t RTS();

  /**
   * Jump to arbitary location.
   */
  uint8_t JMP();

  /**
   * Test bits of accumulator value.
   * Set negative flag if 7th bit is set.
   * Set overflow flag if 6th bit is set.
   * Set zero flag if accumulator is zero.
   */
  uint8_t BIT();

  /**
   * Clear carry flag.
   */
  uint8_t CLC();

  /**
   * Set carry flag.
   */
  uint8_t SEC();

  /**
   * Clear decimal mode flag.
   */
  uint8_t CLD();

  /**
   * Set decimal mode flag.
   */
  uint8_t SED();

  /**
   * Clear inturrept disable flag.
   */
  uint8_t CLI();

  /**
   * Set inturrpet disable flag.
   */
  uint8_t SEI();

  /**
   * Clear overflow flag.
   */
  uint8_t CLV();

  /**
   * Do noting.
   */
  uint8_t NOP();
};
