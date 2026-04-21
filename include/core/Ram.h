#pragma once

#include "Common.h"

class Ram {
private:
    uint8_t WRAM[4 * 1024];
    uint8_t HRAM[127];

public:
    Ram( void );

    uint8_t wram_read( uint16_t addr );
    void wram_write( uint16_t addr, uint8_t data );
    uint8_t hram_read( uint16_t addr );
    void hram_write( uint16_t addr, uint8_t data );
};