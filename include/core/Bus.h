#pragma once

#include <cstdint>
#include <vector>

#include "core/SM83.h"

class Bus {
 private:
  // Fake memory
  std::vector<uint8_t> ram;

 public:
  Bus();
  ~Bus();

  // Emulation
  void reset();
  void clock();

  // Bus devices
  SM83 cpu;

  // Memory access
  void write(uint16_t addr, uint8_t data);
  uint8_t read(uint16_t addr);
};