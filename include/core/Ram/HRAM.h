#pragma once

#include "Common.h"
#include "Logger.h"

#include <array>
#include <algorithm>

class HRAM {
private:
    std::array<uint8_t, 127> memory;

public:
    HRAM();
    ~HRAM();

    void reset();
    uint8_t read( uint16_t addr );
    void write( uint16_t addr, uint8_t data );
    
};