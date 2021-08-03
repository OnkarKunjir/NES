#include "Cpu.hpp"

#include <iostream>

Cpu::Cpu(Bus *bus) : m_bus(bus), m_halt(false), m_cycles(0) {
  m_lookup = {
      {"BRK", &Cpu::BRK, &Cpu::implicit_addressing, 7},
      {"ORA", &Cpu::ORA, &Cpu::indexed_indirect, 6},
      {"KIL", &Cpu::KIL, &Cpu::implicit_addressing, 0},
      {"NOP", &Cpu::NOP, &Cpu::indexed_indirect, 8},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_addressing, 3},
      {"ORA", &Cpu::ORA, &Cpu::zero_page_addressing, 3},
      {"ASL", &Cpu::ASL, &Cpu::zero_page_addressing, 5},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_addressing, 5},
      {"PHP", &Cpu::PHP, &Cpu::implicit_addressing, 3},
      {"ORA", &Cpu::ORA, &Cpu::immediate_addressing, 2},
      {"ASL", &Cpu::ASL, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::immediate_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::absolute_addressing, 4},
      {"ORA", &Cpu::ORA, &Cpu::absolute_addressing, 4},
      {"ASL", &Cpu::ASL, &Cpu::absolute_addressing, 6},
      {"NOP", &Cpu::NOP, &Cpu::absolute_addressing, 6},
      {"BPL", &Cpu::BPL, &Cpu::relative_addressing, 2},
      {"ORA", &Cpu::ORA, &Cpu::indirect_indexed, 5},
      {"KIL", &Cpu::KIL, &Cpu::implicit_addressing, 0},
      {"NOP", &Cpu::NOP, &Cpu::indirect_indexed, 8},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_x_indexed, 4},
      {"ORA", &Cpu::ORA, &Cpu::zero_page_x_indexed, 4},
      {"ASL", &Cpu::ASL, &Cpu::zero_page_x_indexed, 6},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_x_indexed, 6},
      {"CLC", &Cpu::CLC, &Cpu::implicit_addressing, 2},
      {"ORA", &Cpu::ORA, &Cpu::absolute_y_indexed, 4},
      {"NOP", &Cpu::NOP, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::absolute_y_indexed, 7},
      {"NOP", &Cpu::NOP, &Cpu::absolute_x_indexed, 4},
      {"ORA", &Cpu::ORA, &Cpu::absolute_x_indexed, 4},
      {"ASL", &Cpu::ASL, &Cpu::absolute_x_indexed, 7},
      {"NOP", &Cpu::NOP, &Cpu::absolute_x_indexed, 7},
      {"JSR", &Cpu::JSR, &Cpu::absolute_addressing, 6},
      {"AND", &Cpu::AND, &Cpu::indexed_indirect, 6},
      {"KIL", &Cpu::KIL, &Cpu::implicit_addressing, 0},
      {"NOP", &Cpu::NOP, &Cpu::indexed_indirect, 8},
      {"BIT", &Cpu::BIT, &Cpu::zero_page_addressing, 3},
      {"AND", &Cpu::AND, &Cpu::zero_page_addressing, 3},
      {"ROL", &Cpu::ROL, &Cpu::zero_page_addressing, 5},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_addressing, 5},
      {"PLP", &Cpu::PLP, &Cpu::implicit_addressing, 4},
      {"AND", &Cpu::AND, &Cpu::immediate_addressing, 2},
      {"ROL", &Cpu::ROL, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::immediate_addressing, 2},
      {"BIT", &Cpu::BIT, &Cpu::absolute_addressing, 4},
      {"AND", &Cpu::AND, &Cpu::absolute_addressing, 4},
      {"ROL", &Cpu::ROL, &Cpu::absolute_addressing, 6},
      {"NOP", &Cpu::NOP, &Cpu::absolute_addressing, 6},
      {"BMI", &Cpu::BMI, &Cpu::relative_addressing, 2},
      {"AND", &Cpu::AND, &Cpu::indirect_indexed, 5},
      {"KIL", &Cpu::KIL, &Cpu::implicit_addressing, 0},
      {"NOP", &Cpu::NOP, &Cpu::indirect_indexed, 8},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_x_indexed, 4},
      {"AND", &Cpu::AND, &Cpu::zero_page_x_indexed, 4},
      {"ROL", &Cpu::ROL, &Cpu::zero_page_x_indexed, 6},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_x_indexed, 6},
      {"SEC", &Cpu::SEC, &Cpu::implicit_addressing, 2},
      {"AND", &Cpu::AND, &Cpu::absolute_y_indexed, 4},
      {"NOP", &Cpu::NOP, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::absolute_y_indexed, 7},
      {"NOP", &Cpu::NOP, &Cpu::absolute_x_indexed, 4},
      {"AND", &Cpu::AND, &Cpu::absolute_x_indexed, 4},
      {"ROL", &Cpu::ROL, &Cpu::absolute_x_indexed, 7},
      {"NOP", &Cpu::NOP, &Cpu::absolute_x_indexed, 7},
      {"RTI", &Cpu::RTI, &Cpu::implicit_addressing, 6},
      {"EOR", &Cpu::EOR, &Cpu::indexed_indirect, 6},
      {"KIL", &Cpu::KIL, &Cpu::implicit_addressing, 0},
      {"NOP", &Cpu::NOP, &Cpu::indexed_indirect, 8},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_addressing, 3},
      {"EOR", &Cpu::EOR, &Cpu::zero_page_addressing, 3},
      {"LSR", &Cpu::LSR, &Cpu::zero_page_addressing, 5},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_addressing, 5},
      {"PHA", &Cpu::PHA, &Cpu::implicit_addressing, 3},
      {"EOR", &Cpu::EOR, &Cpu::immediate_addressing, 2},
      {"LSR", &Cpu::LSR, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::immediate_addressing, 2},
      {"JMP", &Cpu::JMP, &Cpu::absolute_addressing, 3},
      {"EOR", &Cpu::EOR, &Cpu::absolute_addressing, 4},
      {"LSR", &Cpu::LSR, &Cpu::absolute_addressing, 6},
      {"NOP", &Cpu::NOP, &Cpu::absolute_addressing, 6},
      {"BVC", &Cpu::BVC, &Cpu::relative_addressing, 2},
      {"EOR", &Cpu::EOR, &Cpu::indirect_indexed, 5},
      {"KIL", &Cpu::KIL, &Cpu::implicit_addressing, 0},
      {"NOP", &Cpu::NOP, &Cpu::indirect_indexed, 8},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_x_indexed, 4},
      {"EOR", &Cpu::EOR, &Cpu::zero_page_x_indexed, 4},
      {"LSR", &Cpu::LSR, &Cpu::zero_page_x_indexed, 6},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_x_indexed, 6},
      {"CLI", &Cpu::CLI, &Cpu::implicit_addressing, 2},
      {"EOR", &Cpu::EOR, &Cpu::absolute_y_indexed, 4},
      {"NOP", &Cpu::NOP, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::absolute_y_indexed, 7},
      {"NOP", &Cpu::NOP, &Cpu::absolute_x_indexed, 4},
      {"EOR", &Cpu::EOR, &Cpu::absolute_x_indexed, 4},
      {"LSR", &Cpu::LSR, &Cpu::absolute_x_indexed, 7},
      {"NOP", &Cpu::NOP, &Cpu::absolute_x_indexed, 7},
      {"RTS", &Cpu::RTS, &Cpu::implicit_addressing, 6},
      {"ADC", &Cpu::ADC, &Cpu::indexed_indirect, 6},
      {"KIL", &Cpu::KIL, &Cpu::implicit_addressing, 0},
      {"NOP", &Cpu::NOP, &Cpu::indexed_indirect, 8},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_addressing, 3},
      {"ADC", &Cpu::ADC, &Cpu::zero_page_addressing, 3},
      {"ROR", &Cpu::ROR, &Cpu::zero_page_addressing, 5},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_addressing, 5},
      {"PLA", &Cpu::PLA, &Cpu::implicit_addressing, 4},
      {"ADC", &Cpu::ADC, &Cpu::immediate_addressing, 2},
      {"ROR", &Cpu::ROR, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::immediate_addressing, 2},
      {"JMP", &Cpu::JMP, &Cpu::indirect_addressing, 5},
      {"ADC", &Cpu::ADC, &Cpu::absolute_addressing, 4},
      {"ROR", &Cpu::ROR, &Cpu::absolute_addressing, 6},
      {"NOP", &Cpu::NOP, &Cpu::absolute_addressing, 6},
      {"BVS", &Cpu::BVS, &Cpu::relative_addressing, 2},
      {"ADC", &Cpu::ADC, &Cpu::indirect_indexed, 5},
      {"KIL", &Cpu::KIL, &Cpu::implicit_addressing, 0},
      {"NOP", &Cpu::NOP, &Cpu::indirect_indexed, 8},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_x_indexed, 4},
      {"ADC", &Cpu::ADC, &Cpu::zero_page_x_indexed, 4},
      {"ROR", &Cpu::ROR, &Cpu::zero_page_x_indexed, 6},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_x_indexed, 6},
      {"SEI", &Cpu::SEI, &Cpu::implicit_addressing, 2},
      {"ADC", &Cpu::ADC, &Cpu::absolute_y_indexed, 4},
      {"NOP", &Cpu::NOP, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::absolute_y_indexed, 7},
      {"NOP", &Cpu::NOP, &Cpu::absolute_x_indexed, 4},
      {"ADC", &Cpu::ADC, &Cpu::absolute_x_indexed, 4},
      {"ROR", &Cpu::ROR, &Cpu::absolute_x_indexed, 7},
      {"NOP", &Cpu::NOP, &Cpu::absolute_x_indexed, 7},
      {"NOP", &Cpu::NOP, &Cpu::immediate_addressing, 2},
      {"STA", &Cpu::STA, &Cpu::indexed_indirect, 6},
      {"NOP", &Cpu::NOP, &Cpu::immediate_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::indexed_indirect, 6},
      {"STY", &Cpu::STY, &Cpu::zero_page_addressing, 3},
      {"STA", &Cpu::STA, &Cpu::zero_page_addressing, 3},
      {"STX", &Cpu::STX, &Cpu::zero_page_addressing, 3},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_addressing, 3},
      {"DEY", &Cpu::DEY, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::immediate_addressing, 2},
      {"TXA", &Cpu::TXA, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::immediate_addressing, 2},
      {"STY", &Cpu::STY, &Cpu::absolute_addressing, 4},
      {"STA", &Cpu::STA, &Cpu::absolute_addressing, 4},
      {"STX", &Cpu::STX, &Cpu::absolute_addressing, 4},
      {"NOP", &Cpu::NOP, &Cpu::absolute_addressing, 4},
      {"BCC", &Cpu::BCC, &Cpu::relative_addressing, 2},
      {"STA", &Cpu::STA, &Cpu::indirect_indexed, 6},
      {"KIL", &Cpu::KIL, &Cpu::implicit_addressing, 0},
      {"NOP", &Cpu::NOP, &Cpu::indirect_indexed, 6},
      {"STY", &Cpu::STY, &Cpu::zero_page_x_indexed, 4},
      {"STA", &Cpu::STA, &Cpu::zero_page_x_indexed, 4},
      {"STX", &Cpu::STX, &Cpu::zero_page_y_indexed, 4},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_y_indexed, 4},
      {"TYA", &Cpu::TYA, &Cpu::implicit_addressing, 2},
      {"STA", &Cpu::STA, &Cpu::absolute_y_indexed, 5},
      {"TXS", &Cpu::TXS, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::absolute_y_indexed, 5},
      {"NOP", &Cpu::NOP, &Cpu::absolute_x_indexed, 5},
      {"STA", &Cpu::STA, &Cpu::absolute_x_indexed, 5},
      {"NOP", &Cpu::NOP, &Cpu::absolute_y_indexed, 5},
      {"NOP", &Cpu::NOP, &Cpu::absolute_y_indexed, 5},
      {"LDY", &Cpu::LDY, &Cpu::immediate_addressing, 2},
      {"LDA", &Cpu::LDA, &Cpu::indexed_indirect, 6},
      {"LDX", &Cpu::LDX, &Cpu::immediate_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::indexed_indirect, 6},
      {"LDY", &Cpu::LDY, &Cpu::zero_page_addressing, 3},
      {"LDA", &Cpu::LDA, &Cpu::zero_page_addressing, 3},
      {"LDX", &Cpu::LDX, &Cpu::zero_page_addressing, 3},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_addressing, 3},
      {"TAY", &Cpu::TAY, &Cpu::implicit_addressing, 2},
      {"LDA", &Cpu::LDA, &Cpu::immediate_addressing, 2},
      {"TAX", &Cpu::TAX, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::immediate_addressing, 2},
      {"LDY", &Cpu::LDY, &Cpu::absolute_addressing, 4},
      {"LDA", &Cpu::LDA, &Cpu::absolute_addressing, 4},
      {"LDX", &Cpu::LDX, &Cpu::absolute_addressing, 4},
      {"NOP", &Cpu::NOP, &Cpu::absolute_addressing, 4},
      {"BCS", &Cpu::BCS, &Cpu::relative_addressing, 2},
      {"LDA", &Cpu::LDA, &Cpu::indirect_indexed, 5},
      {"KIL", &Cpu::KIL, &Cpu::implicit_addressing, 0},
      {"NOP", &Cpu::NOP, &Cpu::indirect_indexed, 5},
      {"LDY", &Cpu::LDY, &Cpu::zero_page_x_indexed, 4},
      {"LDA", &Cpu::LDA, &Cpu::zero_page_x_indexed, 4},
      {"LDX", &Cpu::LDX, &Cpu::zero_page_y_indexed, 4},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_y_indexed, 4},
      {"CLV", &Cpu::CLV, &Cpu::implicit_addressing, 2},
      {"LDA", &Cpu::LDA, &Cpu::absolute_y_indexed, 4},
      {"TSX", &Cpu::TSX, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::absolute_y_indexed, 4},
      {"LDY", &Cpu::LDY, &Cpu::absolute_x_indexed, 4},
      {"LDA", &Cpu::LDA, &Cpu::absolute_x_indexed, 4},
      {"LDX", &Cpu::LDX, &Cpu::absolute_y_indexed, 4},
      {"NOP", &Cpu::NOP, &Cpu::absolute_y_indexed, 4},
      {"CPY", &Cpu::CPY, &Cpu::immediate_addressing, 2},
      {"CMP", &Cpu::CMP, &Cpu::indexed_indirect, 6},
      {"NOP", &Cpu::NOP, &Cpu::immediate_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::indexed_indirect, 8},
      {"CPY", &Cpu::CPY, &Cpu::zero_page_addressing, 3},
      {"CMP", &Cpu::CMP, &Cpu::zero_page_addressing, 3},
      {"DEC", &Cpu::DEC, &Cpu::zero_page_addressing, 5},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_addressing, 5},
      {"INY", &Cpu::INY, &Cpu::implicit_addressing, 2},
      {"CMP", &Cpu::CMP, &Cpu::immediate_addressing, 2},
      {"DEX", &Cpu::DEX, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::immediate_addressing, 2},
      {"CPY", &Cpu::CPY, &Cpu::absolute_addressing, 4},
      {"CMP", &Cpu::CMP, &Cpu::absolute_addressing, 4},
      {"DEC", &Cpu::DEC, &Cpu::absolute_addressing, 6},
      {"NOP", &Cpu::NOP, &Cpu::absolute_addressing, 6},
      {"BNE", &Cpu::BNE, &Cpu::relative_addressing, 2},
      {"CMP", &Cpu::CMP, &Cpu::indirect_indexed, 5},
      {"KIL", &Cpu::KIL, &Cpu::implicit_addressing, 0},
      {"NOP", &Cpu::NOP, &Cpu::indirect_indexed, 8},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_x_indexed, 4},
      {"CMP", &Cpu::CMP, &Cpu::zero_page_x_indexed, 4},
      {"DEC", &Cpu::DEC, &Cpu::zero_page_x_indexed, 6},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_x_indexed, 6},
      {"CLD", &Cpu::CLD, &Cpu::implicit_addressing, 2},
      {"CMP", &Cpu::CMP, &Cpu::absolute_y_indexed, 4},
      {"NOP", &Cpu::NOP, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::absolute_y_indexed, 7},
      {"NOP", &Cpu::NOP, &Cpu::absolute_x_indexed, 4},
      {"CMP", &Cpu::CMP, &Cpu::absolute_x_indexed, 4},
      {"DEC", &Cpu::DEC, &Cpu::absolute_x_indexed, 7},
      {"NOP", &Cpu::NOP, &Cpu::absolute_x_indexed, 7},
      {"CPX", &Cpu::CPX, &Cpu::immediate_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::indexed_indirect, 6},
      {"NOP", &Cpu::NOP, &Cpu::immediate_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::indexed_indirect, 8},
      {"CPX", &Cpu::CPX, &Cpu::zero_page_addressing, 3},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_addressing, 3},
      {"INC", &Cpu::INC, &Cpu::zero_page_addressing, 5},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_addressing, 5},
      {"INX", &Cpu::INX, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::immediate_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::immediate_addressing, 2},
      {"CPX", &Cpu::CPX, &Cpu::absolute_addressing, 4},
      {"NOP", &Cpu::NOP, &Cpu::absolute_addressing, 4},
      {"INC", &Cpu::INC, &Cpu::absolute_addressing, 6},
      {"NOP", &Cpu::NOP, &Cpu::absolute_addressing, 6},
      {"BEQ", &Cpu::BEQ, &Cpu::relative_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::indirect_indexed, 5},
      {"KIL", &Cpu::KIL, &Cpu::implicit_addressing, 0},
      {"NOP", &Cpu::NOP, &Cpu::indirect_indexed, 8},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_x_indexed, 4},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_x_indexed, 4},
      {"INC", &Cpu::INC, &Cpu::zero_page_x_indexed, 6},
      {"NOP", &Cpu::NOP, &Cpu::zero_page_x_indexed, 6},
      {"SED", &Cpu::SED, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::absolute_y_indexed, 4},
      {"NOP", &Cpu::NOP, &Cpu::implicit_addressing, 2},
      {"NOP", &Cpu::NOP, &Cpu::absolute_y_indexed, 7},
      {"NOP", &Cpu::NOP, &Cpu::absolute_x_indexed, 4},
      {"NOP", &Cpu::NOP, &Cpu::absolute_x_indexed, 4},
      {"INC", &Cpu::INC, &Cpu::absolute_x_indexed, 7},
      {"NOP", &Cpu::NOP, &Cpu::absolute_x_indexed, 7},
  };
}

void Cpu::log() const {
  std::cout << "Program counter   : " << std::hex << (int)m_pc << "\n";
  std::cout << "Effective address : " << std::hex << (int)m_effective_address
            << "\n";
  std::cout << "Stack pointer     : " << std::hex << (int)m_s << "\n";
  std::cout << "Accumulator       : " << std::hex << (int)m_a << "\n";
  std::cout << "Index X           : " << std::hex << (int)m_x << "\n";
  std::cout << "Index Y           : " << std::hex << (int)m_y << "\n";

  std::cout << "Flags             : N V X B D I Z C\n";
  std::cout << "                    ";
  for (int i = 7; i >= 0; i--) {
    std::cout << (bool)(m_p & (1 << i)) << " ";
  }
  std::cout << "\n";
}

void Cpu::test() {
  Bus bus;
  Cpu cpu(&bus);

  cpu.m_pc = 0xc0fd;

  cpu.m_s = 0x30;

  cpu.m_a = 0x81;
  cpu.m_x = 0x02;
  cpu.m_y = 0x05;

  bus.write(0xc0fd, 0xa9);
  bus.write(0xc0fe, 0xf1);
  cpu.tick();
  cpu.log();

  cpu.tick();
  cpu.log();

  cpu.tick();
  cpu.log();
}

void Cpu::tick() {
  if (!m_cycles) {
    m_opcode = m_bus->read(m_pc++);
    m_cycles = m_lookup[m_opcode].cycles;
    m_cycles += (this->*m_lookup[m_opcode].addressing)();
    m_cycles += (this->*m_lookup[m_opcode].exec)();
  }
  m_cycles--;
}

void Cpu::set_flag(Flag flag, bool value) {
  if (value)
    m_p |= flag;
  else
    m_p &= ~flag;
}

uint8_t Cpu::get_flag(Flag flag) const { return (m_p & flag) > 0 ? 1 : 0; }

uint8_t Cpu::immediate_addressing() {
  m_effective_address = m_pc++;
  return 0;
}

void Cpu::push(uint8_t data) {
  m_bus->write(m_s, data);
  m_s--;
}

uint8_t Cpu::pop() {
  m_s++;
  return m_bus->read(m_s);
}

uint8_t Cpu::implicit_addressing() { return 0; }

uint8_t Cpu::absolute_addressing() {
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

uint8_t Cpu::relative_addressing() {
  m_effective_address = m_bus->read(m_pc++);
  if (m_effective_address & 0x80)
    m_effective_address |= 0xFF00;
  return 0;
}

uint8_t Cpu::absolute_x_indexed() {
  // read low order byte.
  m_effective_address = m_bus->read(m_pc++);
  // read high order byte.
  m_effective_address =
      ((uint16_t)m_bus->read(m_pc++) << 8) | m_effective_address;

  uint8_t page = (m_effective_address & 0xFF00) >> 8;
  m_effective_address += m_x;

  /// Requries one extra cycle if page is changed.
  if (page != ((m_effective_address & 0xFF00) >> 8))
    return 1;

  return 0;
}

uint8_t Cpu::absolute_y_indexed() {
  // read low order byte.
  m_effective_address = m_bus->read(m_pc++);
  // read high order byte.
  m_effective_address =
      ((uint16_t)m_bus->read(m_pc++) << 8) | m_effective_address;

  uint8_t page = (m_effective_address & 0xFF00) >> 8;
  m_effective_address += m_y;

  /// Requries one extra cycle if page is changed.
  if (page != ((m_effective_address & 0xFF00) >> 8))
    return 1;

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

uint8_t Cpu::indirect_addressing() {
  // read the zero page index.
  m_effective_address = m_bus->read(m_pc++);
  m_effective_address = ((uint16_t)m_bus->read(m_effective_address + 1) << 8) |
                        (uint16_t)m_bus->read(m_effective_address);
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
                        m_bus->read((m_effective_address + 1) % 256) * 256;

  uint8_t page = (m_effective_address & 0xFF00) >> 8;
  m_effective_address += m_y;

  if (page != ((m_effective_address & 0xFF00) >> 8))
    return 1;

  return 0;
}

uint8_t Cpu::KIL() {
  m_halt = true;
  return 0;
}

uint8_t Cpu::AND() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_a &= m_fetched_data;
  set_flag(zero, m_a == 0);
  set_flag(negative, m_a & 0x80);
  return 0;
}

uint8_t Cpu::ORA() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_a |= m_fetched_data;
  set_flag(zero, m_a == 0);
  set_flag(negative, m_a & 0x80);
  return 0;
}

uint8_t Cpu::EOR() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_a ^= m_fetched_data;
  set_flag(zero, m_a == 0);
  set_flag(negative, m_a & 0x80);
  return 0;
}

uint8_t Cpu::ADC() {
  uint16_t result = 0;
  m_fetched_data = m_bus->read(m_effective_address);
  result = m_a + m_fetched_data + get_flag(carry);
  /// set appropriate flags.
  set_flag(carry, result > 255);
  set_flag(zero, !(result & 0xFF));
  set_flag(negative, result & 0x80);

  if ((m_a & 0x80) == (m_fetched_data & 0x80)) {
    // if both the numbers have same sign then their is possibility of overflow.
    // check the sign bit of result and accumulator before addition.
    // if sign is changed then set overflow flag else reset.
    set_flag(overflow, (m_a & 0x80) != (result & 0x80));
  }
  // store result in accumulator.
  m_a = result & 0xFF;
  return 0;
}

uint8_t Cpu::SBC() {
  uint16_t result = 0;
  m_fetched_data = m_bus->read(m_effective_address);
  result = m_a - m_fetched_data - get_flag(carry);

  set_flag(carry, (result & 0x80) == 0);
  set_flag(zero, !(result & 0xFF));
  set_flag(negative, result & 0x80);

  if ((m_a & 0x80) != (m_fetched_data & 0x80)) {
    // if both the numbers have different sign then their is possibility of
    // overflow. check the sign bit of result and accumulator before addition.
    // if sign is changed then set overflow flag else reset.
    set_flag(overflow, (m_a & 0x80) != (result & 0x80));
  }
  // store result in accumulator.
  m_a = result & 0xFF;
  return 0;
}

uint8_t Cpu::CMP() {
  uint16_t result = 0;
  m_fetched_data = m_bus->read(m_effective_address);
  result = m_a - m_fetched_data;

  set_flag(zero, !(result & 0xFF));
  set_flag(negative, result & 0x80);
  set_flag(carry, (m_fetched_data <= m_a));
  return 0;
}

uint8_t Cpu::CPX() {
  uint16_t result = 0;
  m_fetched_data = m_bus->read(m_effective_address);
  result = m_x - m_fetched_data;

  set_flag(zero, !(result & 0xFF));
  set_flag(negative, result & 0x80);
  set_flag(carry, m_fetched_data <= m_x);
  return 0;
}

uint8_t Cpu::CPY() {
  uint16_t result = 0;
  m_fetched_data = m_bus->read(m_effective_address);
  result = m_y - m_fetched_data;

  set_flag(zero, !(result & 0xFF));
  set_flag(negative, result & 0x80);
  set_flag(carry, m_fetched_data <= m_y);
  return 0;
}

uint8_t Cpu::DEC() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_fetched_data--;
  m_bus->write(m_effective_address, m_fetched_data);

  set_flag(zero, !m_fetched_data);
  set_flag(negative, m_fetched_data & 0x80);
  return 0;
}

uint8_t Cpu::DEX() {
  m_x--;

  set_flag(zero, !m_x);
  set_flag(negative, m_x & 0x80);
  return 0;
}

uint8_t Cpu::DEY() {
  m_y--;

  set_flag(zero, !m_y);
  set_flag(negative, m_y & 0x80);
  return 0;
}

uint8_t Cpu::INC() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_fetched_data++;
  m_bus->write(m_effective_address, m_fetched_data);

  set_flag(zero, !m_fetched_data);
  set_flag(negative, m_fetched_data & 0x80);
  return 0;
}

uint8_t Cpu::INX() {
  m_x++;

  set_flag(zero, !m_x);
  set_flag(negative, m_x & 0x80);
  return 0;
}

uint8_t Cpu::INY() {
  m_y++;

  set_flag(zero, !m_y);
  set_flag(negative, m_y & 0x80);
  return 0;
}

uint8_t Cpu::ASL() {
  // TODO: implement for implied addressing mode.

  if (m_lookup[m_opcode].addressing == &Cpu::implicit_addressing) {
    m_fetched_data = m_a;
  } else {
    m_fetched_data = m_bus->read(m_effective_address);
  }

  set_flag(carry, m_fetched_data & 0x80);
  m_fetched_data = m_fetched_data << 1;
  set_flag(negative, m_fetched_data & 0x80);
  set_flag(zero, !m_fetched_data);

  if (m_lookup[m_opcode].addressing == &Cpu::implicit_addressing) {
    m_a = m_fetched_data;
  } else {
    m_bus->write(m_effective_address, m_fetched_data);
  }
  return 0;
}

uint8_t Cpu::ROL() {

  if (m_lookup[m_opcode].addressing == &Cpu::implicit_addressing) {
    m_fetched_data = m_a;
  } else {
    m_fetched_data = m_bus->read(m_effective_address);
  }

  uint8_t result = m_fetched_data << 1;
  result = result | get_flag(carry);

  // set flags
  set_flag(carry, m_fetched_data & 0x80);
  set_flag(zero, !result);
  set_flag(negative, result & 0x80);

  if (m_lookup[m_opcode].addressing == &Cpu::implicit_addressing) {
    m_a = result;
  } else {
    m_bus->write(m_effective_address, result);
  }

  return 0;
}

uint8_t Cpu::LSR() {
  if (m_lookup[m_opcode].addressing == &Cpu::implicit_addressing) {
    m_fetched_data = m_a;
  } else {
    m_fetched_data = m_bus->read(m_effective_address);
  }

  set_flag(carry, m_fetched_data & 1);
  m_fetched_data = m_fetched_data >> 1;

  // set flags.
  set_flag(zero, !m_fetched_data);
  set_flag(negative, m_fetched_data & 0x80);

  if (m_lookup[m_opcode].addressing == &Cpu::implicit_addressing) {
    m_a = m_fetched_data;
  } else {
    m_bus->write(m_effective_address, m_fetched_data);
  }
  return 0;
}

uint8_t Cpu::ROR() {
  if (m_lookup[m_opcode].addressing == &Cpu::implicit_addressing) {
    m_fetched_data = m_a;
  } else {
    m_fetched_data = m_bus->read(m_effective_address);
  }

  uint8_t result = m_fetched_data >> 1;
  result = result | (get_flag(carry) << 7);

  // set flags.
  set_flag(carry, m_fetched_data & 1);
  set_flag(zero, !result);
  set_flag(negative, result & 0x80);

  if (m_lookup[m_opcode].addressing == &Cpu::implicit_addressing) {
    m_a = result;
  } else {
    m_bus->write(m_effective_address, result);
  }
  return 0;
}

uint8_t Cpu::LDA() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_a = m_fetched_data;
  set_flag(zero, !m_a);
  set_flag(negative, m_a & 0x80);
  return 0;
}

uint8_t Cpu::STA() {
  m_bus->write(m_effective_address, m_a);
  return 0;
}

uint8_t Cpu::LDX() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_x = m_fetched_data;
  set_flag(zero, !m_x);
  set_flag(negative, m_x & 0x80);
  return 0;
}

uint8_t Cpu::STX() {
  m_bus->write(m_effective_address, m_x);
  return 0;
}

uint8_t Cpu::LDY() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_y = m_fetched_data;
  set_flag(zero, !m_y);
  set_flag(negative, m_y & 0x80);
  return 0;
}

uint8_t Cpu::STY() {
  m_bus->write(m_effective_address, m_y);
  return 0;
}

uint8_t Cpu::TAX() {
  m_x = m_a;
  set_flag(zero, !m_x);
  set_flag(negative, m_x & 0x80);
  return 0;
}

uint8_t Cpu::TXA() {
  m_a = m_x;
  set_flag(zero, !m_a);
  set_flag(negative, m_a & 0x80);
  return 0;
}

uint8_t Cpu::TAY() {
  m_y = m_a;
  set_flag(zero, !m_y);
  set_flag(negative, m_y & 0x80);
  return 0;
}

uint8_t Cpu::TYA() {
  m_a = m_y;
  set_flag(zero, !m_a);
  set_flag(negative, m_a & 0x80);
  return 0;
}

uint8_t Cpu::TSX() {
  m_x = m_s;
  set_flag(zero, !m_x);
  set_flag(negative, m_x & 0x80);
  return 0;
}

uint8_t Cpu::TXS() {
  m_s = m_x;
  set_flag(zero, !m_s);
  set_flag(negative, m_s & 0x80);
  return 0;
}

uint8_t Cpu::PLA() {
  m_a = pop();
  set_flag(zero, !m_a);
  set_flag(negative, m_a & 0x80);
  return 0;
}

uint8_t Cpu::PHA() {
  push(m_a);
  return 0;
}

uint8_t Cpu::PLP() {
  m_p = pop();
  return 0;
}

uint8_t Cpu::PHP() {
  push(m_p);
  return 0;
}

uint8_t Cpu::BPL() {
  if (!get_flag(negative)) {
    uint8_t page = (m_pc & 0xFF00) >> 8;
    m_pc += m_effective_address;
    if (page != ((m_pc & 0xFF00) >> 8))
      return 2;
    return 1;
  }
  return 0;
}

uint8_t Cpu::BMI() {
  if (get_flag(negative)) {
    uint8_t page = (m_pc & 0xFF00) >> 8;
    m_pc += m_effective_address;
    if (page != ((m_pc & 0xFF00) >> 8))
      return 2;
    return 1;
  }
  return 0;
}

uint8_t Cpu::BVC() {
  if (!get_flag(overflow)) {
    uint8_t page = (m_pc & 0xFF00) >> 8;
    m_pc += m_effective_address;
    if (page != ((m_pc & 0xFF00) >> 8))
      return 2;
    return 1;
  }
  return 0;
}

uint8_t Cpu::BVS() {
  if (get_flag(overflow)) {
    uint8_t page = (m_pc & 0xFF00) >> 8;
    m_pc += m_effective_address;
    if (page != ((m_pc & 0xFF00) >> 8))
      return 2;
    return 1;
  }
  return 0;
}

uint8_t Cpu::BCC() {
  if (!get_flag(carry)) {
    uint8_t page = (m_pc & 0xFF00) >> 8;
    m_pc += m_effective_address;
    if (page != ((m_pc & 0xFF00) >> 8))
      return 2;
    return 1;
  }
  return 0;
}

uint8_t Cpu::BCS() {
  if (get_flag(carry)) {
    uint8_t page = (m_pc & 0xFF00) >> 8;
    m_pc += m_effective_address;
    if (page != ((m_pc & 0xFF00) >> 8))
      return 2;
    return 1;
  }
  return 0;
}

uint8_t Cpu::BNE() {
  if (!get_flag(zero)) {
    uint8_t page = (m_pc & 0xFF00) >> 8;
    m_pc += m_effective_address;
    if (page != ((m_pc & 0xFF00) >> 8))
      return 2;
    return 1;
  }
  return 0;
}

uint8_t Cpu::BEQ() {
  if (get_flag(zero)) {
    uint8_t page = (m_pc & 0xFF00) >> 8;
    m_pc += m_effective_address;
    if (page != ((m_pc & 0xFF00) >> 8))
      return 2;
    return 1;
  }
  return 0;
}

uint8_t Cpu::BRK() {

  m_pc++;
  push(m_p);
  // set break flag.
  set_flag(break_command, true);
  // push high order byte of program counter.
  push((m_pc >> 8) & 0xFF);
  // push low order byte of program counter.
  push(m_pc & 0xFF);

  m_pc = m_bus->read(0xFFFE);
  m_pc |= ((uint16_t)m_bus->read(0xFFFF) << 8);
  return 0;
}

uint8_t Cpu::RTI() {
  // read low order byte of program counter.
  m_pc = pop();
  // read high order byte of program counter.
  m_pc = ((uint16_t)pop() << 8) | m_pc;
  // read program status register.
  m_p = pop();
  return 0;
}

uint8_t Cpu::JSR() {
  // push high order byte of program counter.
  push((m_pc >> 8) & 0xFF);
  // push low order byte of program counter.
  push(m_pc & 0xFF);

  m_pc = m_effective_address;
  return 0;
}

uint8_t Cpu::RTS() {
  // read low order byte of program counter.
  m_pc = pop();
  // read high order byte of program counter.
  m_pc = ((uint16_t)pop() << 8) | m_pc;
  return 0;
}

uint8_t Cpu::JMP() {
  m_pc = m_effective_address;
  return 0;
}

uint8_t Cpu::BIT() {
  m_fetched_data = m_bus->read(m_effective_address);
  m_fetched_data &= m_a;
  set_flag(negative, m_fetched_data & 0x80);
  set_flag(overflow, m_fetched_data & 0x40);
  set_flag(zero, !m_fetched_data);
  return 0;
}

uint8_t Cpu::CLC() {
  set_flag(carry, false);
  return 0;
}

uint8_t Cpu::SEC() {
  set_flag(carry, true);
  return 0;
}

uint8_t Cpu::CLD() {
  set_flag(decimal_mode, false);
  return 0;
}

uint8_t Cpu::SED() {
  set_flag(decimal_mode, true);
  return 0;
}
uint8_t Cpu::CLI() {
  set_flag(interrupt_disable, false);
  return 0;
}

uint8_t Cpu::SEI() {
  set_flag(interrupt_disable, true);
  return 0;
}

uint8_t Cpu::CLV() {
  set_flag(overflow, false);
  return 0;
}

uint8_t Cpu::NOP() { return 0; }
