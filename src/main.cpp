#include <filesystem>
#include <iostream>

#include "Engine/Engine.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage: YANGBoy <path/to.rom/file.gb>");
    return 1;
  }

  Engine engine;

  if (!engine.is_initialized()) {
    printf("main.cpp : Could not initialize engine!");
    return 1;
  }

  if (!engine.load_rom(argv[1])) {
    printf("main.cpp : Could not load ROM file [%s]", argv[1]);
    return 1;
  }

  engine.run();

  return 0;
}