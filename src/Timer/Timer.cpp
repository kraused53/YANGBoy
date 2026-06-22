#include "Timer.h"

#include "Bus/Bus.h"
#include "IORegisters.h"
#include "Interrupts.h"

Timer::Timer() {}

Timer::~Timer() {}

void Timer::write_byte(uint16_t addr, uint8_t data) { bus->write(addr, data); }

uint8_t Timer::read_byte(uint16_t addr) { return bus->read(addr); }

void Timer::request_timer_interrupt() { bus->request_interrupt(TIMER_FLAG); }

void Timer::reset() {
  div_counter = 0;
  tima_counter = 0;
}

void Timer::tick(int cycles) {
  div_counter += cycles;

  uint8_t local_tac = read_byte(TAC);

  if (!(local_tac & 0x04)) return;

  uint16_t threshold = tima_thresholds[local_tac & 0x03];
  tima_counter += cycles;

  while (tima_counter >= threshold) {
    tima_counter -= threshold;

    if (read_byte(TIMA) == 0xFF) {
      write_byte(TIMA, read_byte(TMA));
      request_timer_interrupt();
    } else {
      write_byte(TIMA, read_byte(TIMA) + 1);
    }
  }
}

void Timer::clear_div() { div_counter = 0; }

uint8_t Timer::get_div() { return div_counter >> 8; }