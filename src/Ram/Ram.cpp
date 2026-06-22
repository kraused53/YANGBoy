#include "Ram.h"

#include <algorithm>

#include "Addresses.h"

Ram::Ram() {
  wram.resize(8 * 1024);
  hram.resize(127);
  vram.resize(8 * 1024);
  oam.resize(160);

  reset();
}
Ram ::~Ram() {}

void Ram::reset() {
  std::fill(wram.begin(), wram.end(), 0);
  std::fill(hram.begin(), hram.end(), 0);
  std::fill(vram.begin(), vram.end(), 0);
  std::fill(oam.begin(), oam.end(), 0);
  ie = 0;
}

uint8_t Ram::wram_read(uint16_t addr) {
  // Normalize address to wram vector
  addr -= WRAM_START;
  return wram[addr];
}

void Ram::wram_write(uint16_t addr, uint8_t data) {
  addr -= WRAM_START;
  wram[addr] = data;
}

uint8_t Ram::hram_read(uint16_t addr) {
  // Normalize address to hram vector
  addr -= HRAM_START;
  return hram[addr];
}

void Ram::hram_write(uint16_t addr, uint8_t data) {
  addr -= HRAM_START;
  hram[addr] = data;
}

uint8_t Ram::vram_read(uint16_t addr) {
  // Normalize address to hram vector
  addr -= VRAM_START;
  return vram[addr];
}

void Ram::vram_write(uint16_t addr, uint8_t data) {
  addr -= VRAM_START;
  vram[addr] = data;
}

uint8_t Ram::oam_read(uint16_t addr) {
  // Normalize address to hram vector
  addr -= OAM_START;
  return oam[addr];
}

void Ram::oam_write(uint16_t addr, uint8_t data) {
  addr -= OAM_START;
  oam[addr] = data;
}

uint8_t Ram::ie_read() { return ie; }
void Ram::ie_write(uint8_t data) { ie = data; }