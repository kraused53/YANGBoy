#pragma once

#include <cstdint>
#include <string>

#include "IBus.h"
#include "core/Cartridge.h"
#include "core/IO.h"
#include "core/Ram.h"
#include "core/SM83.h"

class Bus : public IBus {
 private:
  std::string test_rom_log;

 public:
  Bus();
  ~Bus();

  // Emulation
  void reset() override;
  void clock() override;
  bool test_done;

  // Bus devices
  SM83 cpu;
  Cartridge cartridge;
  Ram ram;
  IO io;

  // Memory access
  void write(uint16_t addr, uint8_t data) override;
  uint8_t read(uint16_t addr) override;

  // Declare an error
  void handle_bus_error(bool write, uint16_t addr, const char* msg);

  bool is_between(uint16_t addr, uint16_t range_start, uint16_t range_end);
};