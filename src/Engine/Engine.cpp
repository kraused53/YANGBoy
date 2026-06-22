#include "Engine.h"

#include <SDL.h>

struct Engine::screen_data {
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  SDL_Texture* texture = nullptr;
};

Engine::Engine() {
  initialized = false;
  running = false;
  /* Initialization here. Return on error so that initialized stays false. */
  screen = std::make_unique<screen_data>();

  // SDL Setup
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    return;
  }

  screen->window = SDL_CreateWindow("YANGBoy", SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED, 160 * SCALE,
                                    144 * SCALE, SDL_WINDOW_SHOWN);
  if (!screen->window) {
    return;
  }

  screen->renderer = SDL_CreateRenderer(
      screen->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!screen->renderer) {
    return;
  }

  screen->texture =
      SDL_CreateTexture(screen->renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_STREAMING, 160, 144);

  initialized = true;
}

Engine::~Engine() {
  SDL_DestroyTexture(screen->texture);
  SDL_DestroyRenderer(screen->renderer);
  SDL_DestroyWindow(screen->window);
  SDL_Quit();
}

bool Engine::is_initialized() const { return initialized; }

bool Engine::load_rom(const std::filesystem::path& rom_path) {
  return bus.load_rom(rom_path);
}

/* Emulation */
void Engine::setup() {
  /* First time setup */
  running = true;
}

void Engine::input() {
  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
        case SDLK_ESCAPE:
          running = false;
          break;

        case SDLK_a:
          bus.io.update_start(true);
          break;

        case SDLK_s:
          bus.io.update_select(true);
          break;

        case SDLK_z:
          bus.io.update_a(true);
          break;

        case SDLK_x:
          bus.io.update_b(true);
          break;

        case SDLK_LEFT:
          bus.io.update_DPad_l(true);
          break;

        case SDLK_RIGHT:
          bus.io.update_DPad_r(true);
          break;

        case SDLK_UP:
          bus.io.update_DPad_u(true);
          break;

        case SDLK_DOWN:
          bus.io.update_DPad_d(true);
          break;
      }
    } else if (e.type == SDL_KEYUP) {
      switch (e.key.keysym.sym) {
        case SDLK_a:
          bus.io.update_start(false);
          break;

        case SDLK_s:
          bus.io.update_select(false);
          break;

        case SDLK_z:
          bus.io.update_a(false);
          break;

        case SDLK_x:
          bus.io.update_b(false);
          break;

        case SDLK_LEFT:
          bus.io.update_DPad_l(false);
          break;

        case SDLK_RIGHT:
          bus.io.update_DPad_r(false);
          break;

        case SDLK_UP:
          bus.io.update_DPad_u(false);
          break;

        case SDLK_DOWN:
          bus.io.update_DPad_d(false);
          break;
      }
    } else if (e.type == SDL_QUIT) {
      running = false;
    }
  }
}

void Engine::update() {
  for (int i = 0; i < 17556; i++) {
    bus.clock();
  }
}

void Engine::render() {
  SDL_UpdateTexture(screen->texture, nullptr, bus.framebuffer,
                    160 * sizeof(uint32_t));
  SDL_RenderClear(screen->renderer);
  SDL_RenderCopy(screen->renderer, screen->texture, nullptr, nullptr);
  SDL_RenderPresent(screen->renderer);
}

void Engine::run() {
  setup();
  while (running) {
    frameStart = SDL_GetTicks();
    input();
    update();
    render();
    frameTime = SDL_GetTicks() - frameStart;  // Calculate elapsed time

    // If the frame processed faster than our target, delay the thread
    if (TICKS_PER_FRAME > frameTime) {
      SDL_Delay(TICKS_PER_FRAME - frameTime);
    }
  }
}