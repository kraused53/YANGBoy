#pragma once

#include <cstdint>

class IO {
 private:
  // Serial data transfer
  uint8_t iSB;
  uint8_t iSC;

  // Interrupt pending flags
  uint8_t iIF;

  // Timer registers
  // DIV handled by Timer
  uint8_t iTIMA;
  uint8_t iTMA;
  uint8_t iTAC;

  // PPU
  uint8_t iLCDC;
  uint8_t iSTAT;
  uint8_t iSCY;
  uint8_t iSCX;
  uint8_t iLY;
  uint8_t iLYC;
  uint8_t iBGP;
  uint8_t iOBP0;
  uint8_t iOBP1;
  uint8_t iWY;
  uint8_t iWX;

  // Joypad
  uint8_t iP1JOYP;  // Only store upper nibble. Button values are public uints

 public:
  IO();
  ~IO();

  void reset();

  // Joypad
  uint8_t control_buttons;
  uint8_t dpad_buttons;

  void io_write(uint16_t addr, uint8_t data);
  uint8_t io_read(uint16_t addr);
  uint8_t get_if() { return iIF | 0xE0; }
  void set_if(uint8_t value) { iIF = value | 0xE0; }
};