#pragma once

// Extract Information from INSTRUCTION data
#define GET_DST  opcodes[opcode].dst
#define GET_SRC  opcodes[opcode].src
#define GET_COND opcodes[opcode].cond
#define GET_OPT  opcodes[opcode].option
#define CB_GET_DST  cb_opcodes[opcode].dst
#define CB_GET_SRC  cb_opcodes[opcode].src
#define CB_GET_COND cb_opcodes[opcode].cond
#define CB_GET_OPT  cb_opcodes[opcode].option

#include "Common.h"
#include <vector>

// Forward declare Bus class
class Bus;

class SM83 {
private:
    /* Registers */
    uint8_t A;
    uint8_t F;
    uint8_t B;
    uint8_t C;
    uint8_t D;
    uint8_t E;
    uint8_t H;
    uint8_t L;

    /* Pointers */
    uint16_t PC;
    uint16_t SP;
    
    /* Emulation */
    bool halted;
    bool error;
    int ei_pending;
    bool interrupt_enabled;
    int ime_block_counter;
    uint8_t opcode;
    unsigned int cycles;

    /* Memory */
    Bus* bus = nullptr;
    uint8_t   read_byte( uint16_t addr );
    uint16_t  read_word( uint16_t addr );
    uint8_t  fetch_byte( void );
    uint16_t fetch_word( void );
    void     write_byte( uint16_t addr,  uint8_t data );
    void     write_word( uint16_t addr, uint16_t data );

    /* Flags */
    enum FLAG {
        F_Z = ( 1 << 7 ),   // Zero Flag
        F_N = ( 1 << 6 ),   // Negative Flag
        F_H = ( 1 << 5 ),   // Half-Carry Flag
        F_C = ( 1 << 4 )    // Carry Flag
    };
    void set_flag( FLAG f, bool val );
    bool get_flag( FLAG f );
    bool will_carry( uint8_t bit_no, uint16_t a, uint16_t b, bool cy );
    
    /* Registers */
    enum REG {
        RG_NONE,
        RG_A,
        RG_F,
        RG_B,
        RG_C,
        RG_D,
        RG_E,
        RG_H,
        RG_L,
        RP_AF,
        RP_BC,
        RP_DE,
        RP_HL,
        RP_SP
    };
    uint16_t get_register( REG r );
    void     set_register( REG r, uint16_t val );

    // Define Conditionals
	enum COND {
		CN_NONE,    // No condition
		CN_Z,       // If Zero
		CN_NZ,      // If Not Zero
		CN_H,       // If Half
		CN_NH,      // If Not Half
		CN_N,       // If Negative
		CN_NN,      // If Not Negative
		CN_C,       // If Carry
		CN_NC       // If Not Carry
	};
    bool parse_conditional( COND cond );

    /* Parse Interrupts */
    void parse_interrupts( uint8_t interrupts_pending );

    /* Utility */
    uint8_t internal_add( uint8_t a, uint8_t b, bool cy );
    uint16_t pop_from_stack( void );
    void push_to_stack( uint16_t data );

    /* Opcodes */
    void UNI( void );

    void NOP( void );
    void LD_RP_IMM( void );
    void LD_mRP_A( void );
    void INC_RP( void );
    void INC_RG( void );
    void DEC_RG( void );
    void LD_RG_IMM( void );
    void RLCA( void );
    void LD_mRP_SP( void );
    void ADD_HL_RP( void );
    void LD_A_mRP( void );
    void DEC_RP( void );
    void RRCA( void );
    void STOP( void );
    void RLA( void );
    void JUMP_REL( void );
    void RRA( void );
    void DAA( void );
    void CPL( void );
    void SCF( void );
    void CCF( void );
    void MOV_RG_RG( void );
    void MOV_RG_mRP( void );
    void MOV_mRP_RG( void );
    void HALT( void );
    void ADD( void );
    void ADC( void );
    void SUB( void );
    void SBC( void );
    void AND( void );
    void XOR( void );
    void OR( void );
    void CP( void );
    void RET( void );
    void POP( void );
    void JUMP( void );
    void CALL( void );
    void PUSH( void );
    void RST( void );
    void RETI( void );
    void LDH( void );
    void ADD_SP_IMM( void );
    void DI( void );
    void EI( void );
    void LD_SP_HL( void );

    /* CB Opcodes */
    void CB_OPCODES( void );
    void RLC( void );
    void RRC( void );
    void RL( void );
    void RR( void );
    void SLA( void );
    void SRA( void );
    void SWAP( void );
    void SRL( void );
    void BIT( void );
    void RES( void );
    void SET( void );

    /* Instructions */
    struct INSTRUCTION {
        // Pointer to instruction's opcode function
        void ( SM83::*operate )( void ) = nullptr;
        // Define Destination Register
        REG dst;
        // Define Source Register
        REG src;
        // Define Opcode Conditions
        COND cond;
        // Option for CB opcodes
        uint8_t option;
    };
    std::vector<INSTRUCTION> opcodes;
    std::vector<INSTRUCTION> cb_opcodes;

public:
    /* Constructors */
    SM83( void );
    ~SM83( void );

    /* Connect CPU to given bus */
    void connect_to_bus( Bus* b ) { bus = b; }

    /* Getters - For Debugging */
    uint8_t   get_A( void ) { return  A; }
    uint8_t   get_F( void ) { return  F; }
    uint8_t   get_B( void ) { return  B; }
    uint8_t   get_C( void ) { return  C; }
    uint8_t   get_D( void ) { return  D; }
    uint8_t   get_E( void ) { return  E; }
    uint8_t   get_H( void ) { return  H; }
    uint8_t   get_L( void ) { return  L; }
    uint8_t  get_OC( void ) { return  opcode; }
    uint16_t get_PC( void ) { return PC; }
    uint16_t get_SP( void ) { return SP; }
    bool get_halted( void ) { return halted; }
    bool  get_error( void ) { return  error; }
    bool     get_ie( void ) { return  interrupt_enabled; }
    unsigned int get_cycles( void ) { return cycles; }
    void DBG( void );

    /* Emulation */
    int step( void );
    void reset( void );

};