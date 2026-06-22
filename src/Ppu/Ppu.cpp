#include "Ppu.h"

#include "Bus/Bus.h"
#include "IORegisters.h"
#include "Interrupts.h"

PPU::PPU() { reset(); }

PPU::~PPU() {}

uint8_t PPU::read_byte(uint16_t addr) { return bus->read(addr); }

void PPU::write_byte(uint16_t addr, uint8_t data) { bus->write(addr, data); }

void PPU::reset() {
  mode = OAMScan;
  local_ly = 0;
  dot_counter = 0;
  frame_ready = false;
}

void PPU::update_ly() { write_byte(LY, local_ly); }

void PPU::check_lyc() {
  uint8_t stat = bus->read(STAT);
  uint8_t lyc = bus->read(LYC);

  if (local_ly == lyc) {
    stat |= 0x04;  // set coincidence flag (bit 2)
    bus->write(STAT, stat);

    if (stat & 0x40) {  // bit 6 = LYC=LY STAT interrupt enable
      request_stat_interrupt();
    }
  } else {
    stat &= ~0x04;  // clear coincidence flag
    bus->write(STAT, stat);
  }
}

void PPU::request_stat_interrupt() { bus->request_interrupt(LCD_FLAG); }
void PPU::request_vblank_interrupt() { bus->request_interrupt(VBLANK_FLAG); }

void PPU::tick(int cycles) {
  if (!(read_byte(LCDC) & 0x80)) {
    dot_counter = 0;
    local_ly = 0;
    update_ly();
    mode = HBlank;

    // Clear mode bits in STAT
    uint8_t stat = read_byte(STAT);
    stat &= 0xFC;
    write_byte(STAT, stat);
    return;
  }

  dot_counter += cycles;

  switch (mode) {
    case HBlank:
      handle_hblank();
      break;

    case VBlank:
      handle_vblank();
      break;

    case OAMScan:
      handle_oamscan();
      break;

    case PixelTransfer:
      handle_pixeltransfer();
      break;
  }
}

void PPU::handle_hblank() {
  if (dot_counter >= 204) {
    dot_counter -= 204;
    local_ly++;
    update_ly();
    check_lyc();

    if (local_ly == 144) {
      mode = VBlank;
      request_vblank_interrupt();

      frame_ready = true;

      // Update mode bits in STAT (mode 1)
      uint8_t stat = read_byte(STAT);
      stat = (stat & 0xFC) | 0x01;
      write_byte(STAT, stat);

      // Fire STAT interrupt if VBlank STAT interrupt enabled (bit 4)
      if (stat & 0x10) {
        request_stat_interrupt();
      }
    } else {
      mode = OAMScan;

      // Update mode bits in STAT (mode 2)
      uint8_t stat = read_byte(STAT);
      stat = (stat & 0xFC) | 0x02;
      write_byte(STAT, stat);

      // Fire STAT interrupt if OAM interrupt enabled (bit 5)
      if (stat & 0x20) {
        request_stat_interrupt();
      }
    }
  }
}

void PPU::handle_vblank() {
  if (dot_counter >= 456) {
    dot_counter -= 456;
    local_ly++;
    update_ly();
    check_lyc();

    if (local_ly == 154) {
      local_ly = 0;
      update_ly();
      mode = OAMScan;

      // Update mode bits in STAT (mode 2)
      uint8_t stat = read_byte(STAT);
      stat = (stat & 0xFC) | 0x02;
      write_byte(STAT, stat);

      // Fire STAT interrupt if OAM interrupt enabled (bit 5)
      if (stat & 0x20) {
        request_stat_interrupt();
      }
    }
  }
}

void PPU::handle_oamscan() {
  if (dot_counter >= 80) {
    dot_counter -= 80;
    mode = PixelTransfer;

    // Update mode bits in STAT (mode 3)
    uint8_t stat = read_byte(STAT);
    stat = (stat & 0xFC) | 0x03;
    write_byte(STAT, stat);
  }
}

void PPU::handle_pixeltransfer() {
  if (dot_counter >= 172) {
    dot_counter -= 172;
    render_scanline();
    mode = HBlank;

    // Update mode bits in STAT (mode 0)
    uint8_t stat = read_byte(STAT);
    stat = (stat & 0xFC);
    write_byte(STAT, stat);

    // Fire STAT interrupt if HBlank interrupt is enabled (bit 3)
    if (stat & 0x08) {
      request_stat_interrupt();
    }
  }
}

void PPU::render_scanline() {
  uint8_t lcdc = read_byte(LCDC);

  if (lcdc & 0x01) render_background(local_ly);
  if (lcdc & 0x02) render_sprites(local_ly);
}

void PPU::render_background(uint8_t scanline) {
  uint8_t lcdc = read_byte(LCDC);
  uint8_t scy = read_byte(SCY);
  uint8_t scx = read_byte(SCX);
  uint8_t bgp = read_byte(BGP);

  // Which tile map region to use
  uint16_t tile_map = (lcdc & 0x08) ? 0x9C00 : 0x9800;

  // Which tile data region to use — affects how tile index is interpreted
  bool unsigned_addressing = (lcdc & 0x10);
  uint16_t tile_data = unsigned_addressing ? 0x8000 : 0x8800;

  // The actual Y position in the 256x256 background map, accounting for scroll
  uint8_t map_y = scanline + scy;

  // Which row of tiles we're on (each tile is 8px tall)
  uint8_t tile_row = map_y / 8;

  for (int pixel = 0; pixel < 160; pixel++) {
    // The actual X position in the background map
    uint8_t map_x = pixel + scx;

    // Which column of tiles we're on
    uint8_t tile_col = map_x / 8;

    // Look up the tile index from the tile map
    uint16_t tile_map_addr = tile_map + (tile_row * 32) + tile_col;
    uint8_t tile_index = read_byte(tile_map_addr);

    // Get the tile data address
    uint16_t tile_addr;
    if (unsigned_addressing) {
      // 0x8000 mode: tile_index is unsigned 0-255
      tile_addr = tile_data + (tile_index * 16);
    } else {
      // 0x8800 mode: tile_index is signed -128 to 127, base is 0x9000
      tile_addr = 0x9000 + (static_cast<int8_t>(tile_index) * 16);
    }

    // Which row within the tile (each row is 2 bytes)
    uint8_t tile_line = (map_y % 8) * 2;

    uint8_t lo = read_byte(tile_addr + tile_line);
    uint8_t hi = read_byte(tile_addr + tile_line + 1);

    // Which bit within the tile row (bit 7 is leftmost pixel)
    uint8_t bit = 7 - (map_x % 8);

    uint8_t color_id = ((hi >> bit) & 0x01) << 1 | ((lo >> bit) & 0x01);

    framebuffer[scanline * 160 + pixel] = get_shade(bgp, color_id);
  }
}

void PPU::render_sprites(uint8_t scanline) {
  uint8_t lcdc = read_byte(0xFF40);
  uint8_t sprite_height = (lcdc & 0x04) ? 16 : 8;

  // OAM has 40 sprites, 4 bytes each
  for (int i = 0; i < 40; i++) {
    uint16_t oam_addr = 0xFE00 + (i * 4);

    uint8_t sprite_y = read_byte(oam_addr) - 16;
    uint8_t sprite_x = read_byte(oam_addr + 1) - 8;
    uint8_t tile_index = read_byte(oam_addr + 2);
    uint8_t flags = read_byte(oam_addr + 3);

    // Skip sprites not on this scanline
    if (scanline < sprite_y || scanline >= sprite_y + sprite_height) continue;

    bool y_flip = flags & 0x40;
    bool x_flip = flags & 0x20;
    uint8_t palette = (flags & 0x10) ? read_byte(0xFF49) : read_byte(0xFF48);

    // Which row of the sprite we're on
    uint8_t sprite_line = scanline - sprite_y;
    if (y_flip) sprite_line = (sprite_height - 1) - sprite_line;

    // For 8x16 sprites, bit 0 of tile index is ignored
    if (sprite_height == 16) tile_index &= 0xFE;

    uint16_t tile_addr = 0x8000 + (tile_index * 16) + (sprite_line * 2);
    uint8_t lo = read_byte(tile_addr);
    uint8_t hi = read_byte(tile_addr + 1);

    for (int px = 0; px < 8; px++) {
      int screen_x = sprite_x + px;
      if (screen_x < 0 || screen_x >= 160) continue;

      uint8_t bit = x_flip ? px : (7 - px);
      uint8_t color_id = ((hi >> bit) & 0x01) << 1 | ((lo >> bit) & 0x01);

      // Colour ID 0 is transparent for sprites
      if (color_id == 0) continue;

      // Respect BG priority flag
      if ((flags & 0x80) &&
          framebuffer[scanline * 160 + screen_x] != 0xFFFFFFFF)
        continue;

      framebuffer[scanline * 160 + screen_x] = get_shade(palette, color_id);
    }
  }
}

uint32_t PPU::get_shade(uint8_t palette, uint8_t color_id) {
  static constexpr uint32_t colors[4] = {
      0x9BBC0FFF,  // White
      0x8BAC0FFF,  // Light gray
      0x306230FF,  // Dark gray
      0x0F380FFF   // Black
  };
  uint8_t shade = (palette >> (color_id * 2)) & 0x03;
  return colors[shade];
}