#pragma once

#include <cstdint>
#include <vector>

class Ram {
 private:
  std::vector<uint8_t> wram;
  std::vector<uint8_t> hram;
  uint8_t ie;

 public:
  Ram();
  ~Ram();

  void reset();

  uint8_t wram_read(uint16_t addr);
  void wram_write(uint16_t addr, uint8_t data);

  uint8_t hram_read(uint16_t addr);
  void hram_write(uint16_t addr, uint8_t data);

  uint8_t ie_read();
  void ie_write(uint8_t data);
};