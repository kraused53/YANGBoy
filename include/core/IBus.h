#pragma once
#include <cstdint>

// This interface class allows the CPU to connect to a variaty of different bus
// structures for testing
class IBus {
 public:
  virtual uint8_t read(uint16_t addr) = 0;
  virtual void write(uint16_t addr, uint8_t data) = 0;
  virtual void reset() = 0;
  virtual void clock() = 0;
  virtual ~IBus();

  // Interrupts
  virtual uint8_t get_ie() = 0;
  virtual uint8_t get_if() = 0;
  virtual void set_if(uint8_t val) = 0;
};