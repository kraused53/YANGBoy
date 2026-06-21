#include "core/Ram.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <algorithm>

#include "Addresses.h"

Ram::Ram() {
  spdlog::info("Initializing Ram...");
  wram.resize(8 * 1024);
  hram.resize(127);

  reset();
}
Ram ::~Ram() {}

void Ram::reset() {
  spdlog::info("Ram reset...");
  std::fill(wram.begin(), wram.end(), 0);
  std::fill(hram.begin(), hram.end(), 0);
  ie = 0;
}

uint8_t Ram::wram_read(uint16_t addr) {
  // Normalize address to wram vector
  addr -= WRAM_START;
  spdlog::trace("Reading the value {:02X} from WRAM ${:04X}", wram[addr], addr);
  return wram[addr];
}

void Ram::wram_write(uint16_t addr, uint8_t data) {
  addr -= WRAM_START;
  spdlog::trace("Writing the value {:02X} to WRAM ${:04X}", data, addr);
  wram[addr] = data;
}

uint8_t Ram::hram_read(uint16_t addr) {
  // Normalize address to hram vector
  addr -= HRAM_START;
  spdlog::trace("Reading the value {:02X} from HRAM ${:04X}", hram[addr], addr);
  return hram[addr];
}

void Ram::hram_write(uint16_t addr, uint8_t data) {
  addr -= HRAM_START;
  spdlog::trace("Writing the value {:02X} to HRAM ${:04X}", data, addr);
  hram[addr] = data;
}

uint8_t Ram::ie_read() {
  spdlog::trace("Read {:02x} from IE register", ie);
  return ie;
}
void Ram::ie_write(uint8_t data) {
  spdlog::trace("Write {:02x} to IE register", data);
  ie = data;
}