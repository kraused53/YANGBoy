#pragma once

#include "Common.h"
#include "Logger.h"

#include <array>
#include <algorithm>

class WRAM {
private:
    std::array<uint8_t, ( 8 * 1024 )> memory;

public:
    WRAM();
    ~WRAM();

    void reset();
    uint8_t read( uint16_t addr );
    void write( uint16_t addr, uint8_t data );
    
};