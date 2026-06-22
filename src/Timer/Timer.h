#pragma once

#include <cstdint>

class Bus;
class Timer {
 public:
  Timer();
  ~Timer();

  // Bus Access
  void connect_to_bus(Bus* b) { bus = b; }

  void reset();
  void tick(int cycles);

  void clear_div();
  uint8_t get_div();

 private:
  Bus* bus = nullptr;
  void write_byte(uint16_t addr, uint8_t data);
  uint8_t read_byte(uint16_t addr);

  void request_timer_interrupt();

  uint16_t div_counter;
  uint16_t tima_counter;

  static constexpr uint16_t tima_thresholds[4] = {1024, 16, 64, 256};
};