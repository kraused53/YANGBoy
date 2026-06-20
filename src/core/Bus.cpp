#include "core/Bus.h"

Bus::Bus() {
  // Connect CPU to communication bus
  cpu.ConnectBus(this);

  reset();
}

Bus::~Bus() {}

void Bus::write(uint16_t addr, uint8_t data) { ram[addr] = data; }

uint8_t Bus::read(uint16_t addr) { return ram[addr]; }

void Bus::reset() {
  // Clear RAM contents, just in case :P
  for (auto& i : ram) i = 0x00;

  cpu.reset();
}

void Bus::clock() { cpu.step(); }