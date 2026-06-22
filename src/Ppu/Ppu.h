#pragma once

#include <cstdint>

class Bus;
class PPU {
 private:
  Bus* bus = nullptr;
  uint32_t* framebuffer = nullptr;
  uint8_t read_byte(uint16_t addr);
  void write_byte(uint16_t addr, uint8_t data);

  enum Mode { HBlank = 0, VBlank, OAMScan, PixelTransfer };
  Mode mode;

  int local_ly;
  int dot_counter;

  void update_ly();
  void check_lyc();

  void handle_hblank();
  void handle_vblank();
  void handle_oamscan();
  void handle_pixeltransfer();

  void request_stat_interrupt();
  void request_vblank_interrupt();

  void render_scanline();
  void render_background(uint8_t scanline);
  void render_sprites(uint8_t scanline);
  uint32_t get_shade(uint8_t palette, uint8_t color_id);

 public:
  PPU();
  ~PPU();

  void reset();
  void tick(int cycles);

  bool frame_ready;

  void connect_to_bus(Bus* b, uint32_t* fb) {
    bus = b;
    framebuffer = fb;
  }
};