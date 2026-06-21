#pragma once

#include <cstdint>

class IO {
 private:
  // Serial data transfer
  uint8_t sb;
  uint8_t sc;

 public:
  IO();
  ~IO();

  void reset();

  void io_write(uint16_t addr, uint8_t data);
  uint8_t io_read(uint16_t addr);
};