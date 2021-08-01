#include "Cpu.hpp"

#include <iostream>

Cpu::Cpu(Bus *bus) : m_bus(bus) {}

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

  cpu.m_pc = 0xc010;

  cpu.m_s = 0x30;

  cpu.m_a = 0x81;
  cpu.m_x = 0x02;
  cpu.m_y = 0x03;

  bus.write(0xc010, 0x05);
  bus.write(0xc011, 0xff);
  bus.write(0xff04, 0x00);

  bus.write(0xfffe, 0xff);
  bus.write(0xffff, 0x20);

  bus.write(0xff05, 0xc0);

  // cpu.absolute_addressing();
  cpu.NOP();
  cpu.log();

  // std::cout << std::hex << (int)bus.read(0xff04) << "\n";
  // std::cout << std::hex << (int)cpu.pop() << "\n";
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
    m_effective_address |= 0xFF;
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
  m_fetched_data = m_bus->read(m_effective_address);
  set_flag(carry, m_fetched_data & 0x80);
  m_fetched_data = m_fetched_data << 1;
  set_flag(negative, m_fetched_data & 0x80);
  set_flag(zero, !m_fetched_data);
  m_bus->write(m_effective_address, m_fetched_data);
  return 0;
}

uint8_t Cpu::ROL() {
  m_fetched_data = m_bus->read(m_effective_address);
  uint8_t result = m_fetched_data << 1;
  result = result | get_flag(carry);

  // set flags
  set_flag(carry, m_fetched_data & 0x80);
  set_flag(zero, !result);
  set_flag(negative, result & 0x80);

  m_bus->write(m_effective_address, result);
  return 0;
}

uint8_t Cpu::LSR() {
  m_fetched_data = m_bus->read(m_effective_address);
  set_flag(carry, m_fetched_data & 1);
  m_fetched_data = m_fetched_data >> 1;

  // set flags.
  set_flag(zero, !m_fetched_data);
  set_flag(negative, m_fetched_data & 0x80);

  m_bus->write(m_effective_address, m_fetched_data);
  return 0;
}

uint8_t Cpu::ROR() {
  m_fetched_data = m_bus->read(m_effective_address);
  uint8_t result = m_fetched_data >> 1;
  result = result | (get_flag(carry) << 7);

  // set flags.
  set_flag(carry, m_fetched_data & 1);
  set_flag(zero, !result);
  set_flag(negative, result & 0x80);

  m_bus->write(m_effective_address, result);
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
    m_pc += m_effective_address;
  }
  return 0;
}

uint8_t Cpu::BMI() {
  if (get_flag(negative)) {
    m_pc += m_effective_address;
  }
  return 0;
}

uint8_t Cpu::BVC() {
  if (!get_flag(overflow)) {
    m_pc += m_effective_address;
  }
  return 0;
}

uint8_t Cpu::BVS() {
  if (get_flag(overflow)) {
    m_pc += m_effective_address;
  }
  return 0;
}

uint8_t Cpu::BCC() {
  if (!get_flag(carry)) {
    m_pc += m_effective_address;
  }
  return 0;
}

uint8_t Cpu::BCS() {
  if (get_flag(carry)) {
    m_pc += m_effective_address;
  }
  return 0;
}

uint8_t Cpu::BNE() {
  if (!get_flag(zero)) {
    m_pc += m_effective_address;
  }
  return 0;
}

uint8_t Cpu::BEQ() {
  if (get_flag(zero)) {
    m_pc += m_effective_address;
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
