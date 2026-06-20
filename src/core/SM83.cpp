#include "core/SM83.h"

#include "core/Bus.h"

SM83::SM83() { reset(); };

uint8_t SM83::read(uint16_t addr) { return bus->read(addr); }
void SM83::write(uint16_t addr, uint8_t data) { bus->write(addr, data); }

// Emulation
void SM83::reset() {
  // Initialize registers to 0
  reg.af = 0x0000;
  reg.bc = 0x0000;
  reg.de = 0x0000;
  reg.hl = 0x0000;
  reg.pc = 0x0000;
  reg.sp = 0x0000;
}
uint8_t SM83::step() { return 1; }