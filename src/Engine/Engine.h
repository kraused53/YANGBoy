#pragma once

#include <filesystem>
#include <string>

#include "Bus/Bus.h"

class Engine {
 private:
  // Emulation
  bool initialized;
  bool running;
  void setup();
  void input();
  void update();
  void render();

  // Gameboy System

  // Screen Settings
  uint8_t SCALE = 3;
  struct screen_data;
  std::unique_ptr<screen_data> screen;
  const int TARGET_FPS = 60;
  const int TICKS_PER_FRAME = 1000 / TARGET_FPS;
  uint32_t frameStart;
  int frameTime;

  Bus bus;

 public:
  Engine();
  ~Engine();

  bool load_rom(const std::filesystem::path& rom_path);
  bool is_initialized() const;

  void run();
};