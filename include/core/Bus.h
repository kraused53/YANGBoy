#pragma once

#include <cstdint>
#include <vector>

#include "IBus.h"
#include "core/SM83.h"

class Bus : public IBus {
 public:
  Bus();
  ~Bus();

  // Emulation
  void reset() override;
  void clock() override;

  // Bus devices
  SM83 cpu;

  // Memory access
  void write(uint16_t addr, uint8_t data) override;
  uint8_t read(uint16_t addr) override;

  // Fake memory
  std::vector<uint8_t> ram;
};