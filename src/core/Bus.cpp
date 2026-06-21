#include "core/Bus.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

Bus::Bus() {
  // Connect CPU to communication bus
  cpu.connect_to_bus(this);

  ram.resize(0x10000);  // 64KB of addressable memory

  reset();
}

Bus::~Bus() {}

void Bus::write(uint16_t addr, uint8_t data) {
  spdlog::trace("       Write] ${:04x} -> {:02X}", addr, data);
  ram[addr] = data;
}

uint8_t Bus::read(uint16_t addr) {
  spdlog::trace("        Read] ${:04x} -> {:02X}", addr, ram[addr]);
  return ram[addr];
}

void Bus::reset() {
  spdlog::info("Full system reset...");
  // Clear RAM contents, just in case :P
  for (auto& i : ram) i = 0x00;

  cpu.reset();
}

void Bus::clock() { cpu.step(); }