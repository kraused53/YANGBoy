#pragma once

#include <cstdint>

struct Registers {
  union {
    struct {
      uint8_t c;
      uint8_t b;
    };
    uint16_t bc;
  };

  union {
    struct {
      uint8_t e;
      uint8_t d;
    };
    uint16_t de;
  };

  union {
    struct {
      uint8_t l;
      uint8_t h;
    };
    uint16_t hl;
  };

  union {
    struct {
      uint8_t f;
      uint8_t a;
    };
    uint16_t af;
  };

  uint16_t pc;
  uint16_t sp;
};

class Bus;
class SM83 {
 public:
  SM83();

  // System registers
  Registers reg;

  // System bus
  Bus* bus = nullptr;
  uint8_t read(uint16_t addr);
  void write(uint16_t addr, uint8_t data);
  void ConnectBus(Bus* n) { bus = n; }

  // Emulation
  void reset();
  uint8_t step();
};