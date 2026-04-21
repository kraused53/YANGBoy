#pragma once

#include <Common.h>

class Bus;

class Timer {
private:

    Bus* bus = nullptr;

    uint16_t DIV;
    uint8_t TIMA;
    uint8_t TMA;
    uint8_t TAC;

    uint16_t div_counter;
    uint8_t tima_counter;

    uint16_t get_timer_period( void );
    void step_tima( void );
public:
    Timer();
    ~Timer();

    void write_DIV( uint8_t data );
    void write_TIMA( uint8_t data );
    void write_TMA( uint8_t data );
    void write_TAC( uint8_t data );

    void connect_to_bus( Bus* b ) { bus = b; }

    uint8_t read_DIV( void );
    uint8_t read_TIMA( void );
    uint8_t read_TMA( void );
    uint8_t read_TAC( void );

    void clock( int cycles );
    void reset( void );
};