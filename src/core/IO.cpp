#include "core/IO.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

IO::IO() {
  spdlog::info("Initializing IO...");
  reset();
}

IO::~IO() {}

void IO::reset() {
  spdlog::info("IO Reset...");
  // Reset Serial control
  sb = 0;
  sc = 0;
}

uint8_t IO::io_read(uint16_t addr) {
  uint8_t data = 0x00;
  switch (addr) {
    // Serial data transfer
    case 0xFF01:
      spdlog::trace("IO: Reading SB ({:02X})", sb);
      data = sb;
      break;
    case 0xFF02:
      spdlog::trace("IO: Reading SC ({:02X})", sc);
      data = sc;
      break;

    // Not implemented yet / error
    default:
      spdlog::warn("This IO register has not been implemented yet...");
      break;
  }
  return data;
}

void IO::io_write(uint16_t addr, uint8_t data) {
  switch (addr) {
    // Serial data transfer
    case 0xFF01:
      spdlog::trace("IO: Write to SB -> {:02X}", data);
      sb = data;
      break;
    case 0xFF02:
      spdlog::trace("IO: Write to SC -> {:02X}", data);
      sc = data;
      break;

    // Not implemented yet / error
    default:
      spdlog::warn("This IO register has not been implemented yet...");
      break;
  }
}