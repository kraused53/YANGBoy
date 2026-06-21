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
  // Reset registers
  SB = 0;
  SC = 0;
  IF = 0xE1;
  DIV = 0xABCC;
  TIMA = 0x00;
  TMA = 0x00;
  TAC = 0xF8;
  tima_counter = 0;
  timer_interrupt = false;
  JOYP = 0x00;
}

uint8_t IO::io_read(uint16_t addr) {
  uint8_t data = 0x00;
  switch (addr) {
    // Serial data transfer
    case 0xFF01:
      spdlog::trace("IO: Reading SB ({:02X})", SB);
      data = SB;
      break;
    case 0xFF02:
      spdlog::trace("IO: Reading SC ({:02X})", SC);
      data = SC;
      break;

    // Interrupt flags
    case 0xFF0F:
      spdlog::trace("IO: Reading IF ({:02X})", IF);
      data = IF | 0xE0;
      break;

      // Timer
    case 0xFF03:
      spdlog::trace("IO: Reading DIV ({:02X})", DIV >> 8);
      data = DIV >> 8;
      break;

    case 0xFF05:
      spdlog::trace("IO: Reading TIMA ({:02X})", TIMA);
      data = TIMA;
      break;

    case 0xFF06:
      spdlog::trace("IO: Reading TMA ({:02X})", TMA);
      data = TMA;
      break;

    case 0xFF07:
      spdlog::trace("IO: Reading TAC ({:02X})", TAC & 0xF8);
      data = TAC | 0xF8;
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
      SB = data;
      break;
    case 0xFF02:
      spdlog::trace("IO: Write to SC -> {:02X}", data);
      SC = data;
      break;

      // Interrupt flags
    case 0xFF0F:
      spdlog::trace("IO: Write to IF -> {:02X}", data);
      IF = data | 0xE0;
      break;

      // Timer
    case 0xFF03:
      spdlog::trace("IO: Write to DIV -> {:02X}", data);
      DIV = 0;  // Any write resets to zero
      break;

    case 0xFF05:
      spdlog::trace("IO: Write to TIMA -> {:02X}", data);
      TIMA = data;
      break;

    case 0xFF06:
      spdlog::trace("IO: Write to TMA -> {:02X}", data);
      TMA = data;
      break;

    case 0xFF07:
      spdlog::trace("IO: Write to TAC -> {:02X}", data);
      TAC = data & 0x07;
      break;

    // Not implemented yet / error
    default:
      spdlog::warn("This IO register has not been implemented yet...");
      break;
  }
}

void IO::timer_tick(int cycles) {
  DIV += cycles;  // DIV is just bits 8-15 of this counter

  if (!(TAC & 0x04)) return;  // timer disabled

  uint16_t threshold = tima_thresholds[TAC & 0x03];
  tima_counter += cycles;

  while (tima_counter >= threshold) {
    tima_counter -= threshold;
    if (TIMA == 0xFF) {
      TIMA = TMA;  // reload from TMA on overflow
      timer_interrupt = true;
    } else {
      TIMA++;
    }
  }
}