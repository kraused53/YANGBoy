#pragma once

#include <cstdint>
#include <string>

#include "Bus/IBus.h"
#include "Cartridge/Cartridge.h"
#include "Cpu/SM83.h"
#include "IO/IO.h"
#include "Interrupts.h"
#include "Ppu/Ppu.h"
#include "Ram/Ram.h"
#include "Timer/Timer.h"

class Bus : public IBus {
 private:
  std::string test_rom_log;

  void dma_transfer(uint8_t data);

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
  Timer timer;
  PPU ppu;

  uint32_t framebuffer[160 * 144] = {};

  // Memory access
  void write(uint16_t addr, uint8_t data) override;
  uint8_t read(uint16_t addr) override;

  // Utility
  bool is_between(uint16_t addr, uint16_t range_start, uint16_t range_end);
  bool load_rom(const std::filesystem::path& rom_path);

  // Interrupts
  uint8_t get_ie() override;
  uint8_t get_if() override;
  void set_if(uint8_t val) override;
  void request_interrupt(uint8_t interrupt_flag);
};