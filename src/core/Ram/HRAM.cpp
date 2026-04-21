#include "core/Ram/HRAM.h"

HRAM::HRAM() {
    reset();
}


HRAM::~HRAM() {
    
}

void HRAM::reset() {
    memory.fill( 0x00 );
}

uint8_t HRAM::read( uint16_t addr ) {
    return memory[addr - HRAM_START];
}

void HRAM::write( uint16_t addr, uint8_t data ) {
    memory[addr - HRAM_START] = data;
}