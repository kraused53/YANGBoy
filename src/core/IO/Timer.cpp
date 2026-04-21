#include "core/IO/Timer.h"
#include "core/Bus.h"
#include "Interrupts.h"

Timer::Timer() {}
Timer::~Timer() {}

void Timer::write_DIV( uint8_t data ) {
    DIV = 0;
    div_counter = 0;
}

void Timer::write_TIMA( uint8_t data ) {
    TIMA = data;
}

void Timer::write_TMA( uint8_t data ) {
    TMA = data;
}

void Timer::write_TAC( uint8_t data ) {
    TAC = data;
}

uint8_t Timer::read_DIV( void ) {
    return DIV >> 8;
}

uint8_t Timer::read_TIMA( void ) {
    return TIMA;
}

uint8_t Timer::read_TMA( void ) {
    return TMA;
}

uint8_t Timer::read_TAC( void ) {
    return TAC;
}

void Timer::clock( int cycles ) {
    div_counter += cycles;

    while ( div_counter >= 256 ) {
        div_counter -= 256;
        DIV++;
    }
    
    if( TAC & 0x04 ) {
        tima_counter += cycles;

        uint16_t period = get_timer_period();

        while( tima_counter >= period ) {
            tima_counter -= period;
            step_tima();
        }
    }
}

uint16_t Timer::get_timer_period( void ) {
    switch( TAC & 0x03 ) {
        case 0: return 1024;
        case 1: return 16;
        case 2: return 64;
        case 3: return 256;
    }
    return 1024;
}

void Timer::step_tima( void ) {
    if( TIMA == 0xFF ) {
        TIMA = TMA;
        bus->request_interrupt( IT_TIMER );
    }else {
        TIMA++;
    }
}

void Timer::reset() {

}