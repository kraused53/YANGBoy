#pragma once

#include <cstdint>

class IO {
 private:
  // Serial data transfer
  uint8_t SB;
  uint8_t SC;

  // Interrupt pending flags
  uint8_t IF;

  // Timer registers
  uint16_t DIV;
  uint8_t TIMA;
  uint8_t TMA;
  uint8_t TAC;
  static constexpr uint16_t tima_thresholds[4] = {1024, 16, 64, 256};
  uint16_t tima_counter;

  // Joypad
  uint8_t JOYP;

 public:
  IO();
  ~IO();

  void reset();
  bool timer_interrupt;

  void io_write(uint16_t addr, uint8_t data);
  uint8_t io_read(uint16_t addr);
  uint8_t get_if() { return IF | 0xE0; }
  void set_if(uint8_t value) { IF = value | 0xE0; }

  void timer_tick(int cycles);
};