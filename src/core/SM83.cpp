#include "core/SM83.h"
#include "core/Bus.h"
#include "Interrupts.h"

/* Constructors */
SM83::SM83( void ) {
    Logger::Log( "Initializing SM83 CPU..." );
    reset();

    // Alias class name to make lines shorter
    using s = SM83;

    // Define Standard Opcodes
    opcodes = {
        // 0x00 -> 0x0F
        { &s::NOP,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x00 - NOP
        { &s::LD_RP_IMM,   RP_BC, RG_NONE, CN_NONE, 0 }, // 0x01 - LD BC, d16
        { &s::LD_mRP_A,    RP_BC, RG_NONE, CN_NONE, 0 }, // 0x02 - LD [BC], A
        { &s::INC_RP,      RP_BC, RG_NONE, CN_NONE, 0 }, // 0x03 - INC BC
        { &s::INC_RG,       RG_B, RG_NONE, CN_NONE, 0 }, // 0x04 - INC B
        { &s::DEC_RG,       RG_B, RG_NONE, CN_NONE, 0 }, // 0x05 - DEC B
        { &s::LD_RG_IMM,    RG_B, RG_NONE, CN_NONE, 0 }, // 0x06 - LD B, d8
        { &s::RLCA,      RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x07 - RLCA
        { &s::LD_mRP_SP, RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x08 - LD [a16], SP
        { &s::ADD_HL_RP, RG_NONE,   RP_BC, CN_NONE, 0 }, // 0x09 - ADD HL, BC
        { &s::LD_A_mRP,  RG_NONE,   RP_BC, CN_NONE, 0 }, // 0x0A - LD A, [BC]
        { &s::DEC_RP,      RP_BC, RG_NONE, CN_NONE, 0 }, // 0x0B - DEC BC
        { &s::INC_RG,       RG_C, RG_NONE, CN_NONE, 0 }, // 0x0C - INC C
        { &s::DEC_RG,       RG_C, RG_NONE, CN_NONE, 0 }, // 0x0D - DEC C
        { &s::LD_RG_IMM,    RG_C, RG_NONE, CN_NONE, 0 }, // 0x0E - LD C, d8
        { &s::RRCA,      RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x0F - RRCA
        // 0x10 -> 0x1F
        { &s::STOP,      RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x10 - STOP
        { &s::LD_RP_IMM,   RP_DE, RG_NONE, CN_NONE, 0 }, // 0x11 - LD DE, d16
        { &s::LD_mRP_A,    RP_DE, RG_NONE, CN_NONE, 0 }, // 0x12 - LD [DE], A
        { &s::INC_RP,      RP_DE, RG_NONE, CN_NONE, 0 }, // 0x13 - INC DE
        { &s::INC_RG,       RG_D, RG_NONE, CN_NONE, 0 }, // 0x14 - INC D
        { &s::DEC_RG,       RG_D, RG_NONE, CN_NONE, 0 }, // 0x15 - DEC D
        { &s::LD_RG_IMM,    RG_D, RG_NONE, CN_NONE, 0 }, // 0x06 - LD D, d8
        { &s::RLA,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x17 - RLA
        { &s::JUMP_REL,  RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x18 - JR a8
        { &s::ADD_HL_RP, RG_NONE,   RP_DE, CN_NONE, 0 }, // 0x19 - ADD HL, DE
        { &s::LD_A_mRP,  RG_NONE,   RP_DE, CN_NONE, 0 }, // 0x1A - LD A, [DE]
        { &s::DEC_RP,      RP_DE, RG_NONE, CN_NONE, 0 }, // 0x1B - DEC DE
        { &s::INC_RG,       RG_E, RG_NONE, CN_NONE, 0 }, // 0x1C - INC E
        { &s::DEC_RG,       RG_E, RG_NONE, CN_NONE, 0 }, // 0x1D - DEC E
        { &s::LD_RG_IMM,    RG_E, RG_NONE, CN_NONE, 0 }, // 0x1E - LD E, d8
        { &s::RRA,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x1F - RRA
        // 0x20 -> 0x2F
        { &s::JUMP_REL,  RG_NONE, RG_NONE,   CN_NZ, 0 }, // 0x20 - JR NZ, a8
        { &s::LD_RP_IMM,   RP_HL, RG_NONE, CN_NONE, 0 }, // 0x21 - LD HL, d16
        { &s::LD_mRP_A,    RP_HL, RG_NONE, CN_NONE, 0 }, // 0x22 - LD [HL+], A
        { &s::INC_RP,      RP_HL, RG_NONE, CN_NONE, 0 }, // 0x23 - INC HL
        { &s::INC_RG,       RG_H, RG_NONE, CN_NONE, 0 }, // 0x24 - INC H
        { &s::DEC_RG,       RG_H, RG_NONE, CN_NONE, 0 }, // 0x25 - DEC H
        { &s::LD_RG_IMM,    RG_H, RG_NONE, CN_NONE, 0 }, // 0x26 - LD H, d8
        { &s::DAA,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x27 - DAA
        { &s::JUMP_REL,  RG_NONE, RG_NONE,    CN_Z, 0 }, // 0x28 - JR Z, a8
        { &s::ADD_HL_RP, RG_NONE,   RP_HL, CN_NONE, 0 }, // 0x29 - ADD HL, HL
        { &s::LD_A_mRP,  RG_NONE,   RP_HL, CN_NONE, 0 }, // 0x2A - LD A, [HL+]
        { &s::DEC_RP,      RP_HL, RG_NONE, CN_NONE, 0 }, // 0x2B - DEC HL
        { &s::INC_RG,       RG_L, RG_NONE, CN_NONE, 0 }, // 0x2C - INC L
        { &s::DEC_RG,       RG_L, RG_NONE, CN_NONE, 0 }, // 0x2D - DEC L
        { &s::LD_RG_IMM,    RG_L, RG_NONE, CN_NONE, 0 }, // 0x2E - LD L, d8
        { &s::CPL,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x2F - CPL
        // 0x30 -> 0x3F
        { &s::JUMP_REL,  RG_NONE, RG_NONE,   CN_NC, 0 }, // 0x30 - JR NC, a8
        { &s::LD_RP_IMM,   RP_SP, RG_NONE, CN_NONE, 0 }, // 0x31 - LD SP, d16
        { &s::LD_mRP_A,    RP_HL, RG_NONE, CN_NONE, 0 }, // 0x32 - LD [HL-], A
        { &s::INC_RP,      RP_SP, RG_NONE, CN_NONE, 0 }, // 0x33 - INC SP
        { &s::INC_RG,    RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x34 - INC [HL]
        { &s::DEC_RG,    RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x35 - DEC [HL]
        { &s::LD_RG_IMM, RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x36 - LD [HL], d8
        { &s::SCF,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x37 - SCF
        { &s::JUMP_REL,  RG_NONE, RG_NONE,    CN_C, 0 }, // 0x38 - JR C, a8
        { &s::ADD_HL_RP, RG_NONE,   RP_SP, CN_NONE, 0 }, // 0x39 - ADD HL, SP
        { &s::LD_A_mRP,  RG_NONE,   RP_HL, CN_NONE, 0 }, // 0x3A - LD A, [HL-]
        { &s::DEC_RP,      RP_SP, RG_NONE, CN_NONE, 0 }, // 0x3B - DEC SP
        { &s::INC_RG,       RG_A, RG_NONE, CN_NONE, 0 }, // 0x3C - INC A
        { &s::DEC_RG,       RG_A, RG_NONE, CN_NONE, 0 }, // 0x3D - DEC A
        { &s::LD_RG_IMM,    RG_A, RG_NONE, CN_NONE, 0 }, // 0x3E - LD A, d8
        { &s::CCF,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x3F - CCF
        // 0x40 -> 0x4F
        { &s::MOV_RG_RG,    RG_B,    RG_B, CN_NONE, 0 }, // 0x40 - LD B, B
        { &s::MOV_RG_RG,    RG_B,    RG_C, CN_NONE, 0 }, // 0x41 - LD B, C
        { &s::MOV_RG_RG,    RG_B,    RG_D, CN_NONE, 0 }, // 0x42 - LD B, D
        { &s::MOV_RG_RG,    RG_B,    RG_E, CN_NONE, 0 }, // 0x43 - LD B, E
        { &s::MOV_RG_RG,    RG_B,    RG_H, CN_NONE, 0 }, // 0x44 - LD B, H
        { &s::MOV_RG_RG,    RG_B,    RG_L, CN_NONE, 0 }, // 0x45 - LD B, L
        { &s::MOV_RG_mRP,   RG_B, RG_NONE, CN_NONE, 0 }, // 0x46 - LD B, [HL]
        { &s::MOV_RG_RG,    RG_B,    RG_A, CN_NONE, 0 }, // 0x47 - LD B, A
        { &s::MOV_RG_RG,    RG_C,    RG_B, CN_NONE, 0 }, // 0x48 - LD C, B
        { &s::MOV_RG_RG,    RG_C,    RG_C, CN_NONE, 0 }, // 0x49 - LD C, C
        { &s::MOV_RG_RG,    RG_C,    RG_D, CN_NONE, 0 }, // 0x4A - LD C, D
        { &s::MOV_RG_RG,    RG_C,    RG_E, CN_NONE, 0 }, // 0x4B - LD C, E
        { &s::MOV_RG_RG,    RG_C,    RG_H, CN_NONE, 0 }, // 0x4C - LD C, H
        { &s::MOV_RG_RG,    RG_C,    RG_L, CN_NONE, 0 }, // 0x4D - LD C, L
        { &s::MOV_RG_mRP,   RG_C, RG_NONE, CN_NONE, 0 }, // 0x4E - LD C, [HL]
        { &s::MOV_RG_RG,    RG_C,    RG_A, CN_NONE, 0 }, // 0x4F - LD C, A
        // 0x50 -> 0x5F
        { &s::MOV_RG_RG,    RG_D,    RG_B, CN_NONE, 0 }, // 0x50 - LD D, B
        { &s::MOV_RG_RG,    RG_D,    RG_C, CN_NONE, 0 }, // 0x51 - LD D, C
        { &s::MOV_RG_RG,    RG_D,    RG_D, CN_NONE, 0 }, // 0x52 - LD D, D
        { &s::MOV_RG_RG,    RG_D,    RG_E, CN_NONE, 0 }, // 0x53 - LD D, E
        { &s::MOV_RG_RG,    RG_D,    RG_H, CN_NONE, 0 }, // 0x54 - LD D, H
        { &s::MOV_RG_RG,    RG_D,    RG_L, CN_NONE, 0 }, // 0x55 - LD D, L
        { &s::MOV_RG_mRP,   RG_D, RG_NONE, CN_NONE, 0 }, // 0x56 - LD D, [HL]
        { &s::MOV_RG_RG,    RG_D,    RG_A, CN_NONE, 0 }, // 0x57 - LD D, A
        { &s::MOV_RG_RG,    RG_E,    RG_B, CN_NONE, 0 }, // 0x58 - LD E, B
        { &s::MOV_RG_RG,    RG_E,    RG_C, CN_NONE, 0 }, // 0x59 - LD E, C
        { &s::MOV_RG_RG,    RG_E,    RG_D, CN_NONE, 0 }, // 0x5A - LD E, D
        { &s::MOV_RG_RG,    RG_E,    RG_E, CN_NONE, 0 }, // 0x5B - LD E, E
        { &s::MOV_RG_RG,    RG_E,    RG_H, CN_NONE, 0 }, // 0x5C - LD E, H
        { &s::MOV_RG_RG,    RG_E,    RG_L, CN_NONE, 0 }, // 0x5D - LD E, L
        { &s::MOV_RG_mRP,   RG_E, RG_NONE, CN_NONE, 0 }, // 0x5E - LD E, [HL]
        { &s::MOV_RG_RG,    RG_E,    RG_A, CN_NONE, 0 }, // 0x5F - LD E, A
        // 0x60 -> 0x6F
        { &s::MOV_RG_RG,    RG_H,    RG_B, CN_NONE, 0 }, // 0x60 - LD H, B
        { &s::MOV_RG_RG,    RG_H,    RG_C, CN_NONE, 0 }, // 0x61 - LD H, C
        { &s::MOV_RG_RG,    RG_H,    RG_D, CN_NONE, 0 }, // 0x62 - LD H, D
        { &s::MOV_RG_RG,    RG_H,    RG_E, CN_NONE, 0 }, // 0x63 - LD H, E
        { &s::MOV_RG_RG,    RG_H,    RG_H, CN_NONE, 0 }, // 0x64 - LD H, H
        { &s::MOV_RG_RG,    RG_H,    RG_L, CN_NONE, 0 }, // 0x65 - LD H, L
        { &s::MOV_RG_mRP,   RG_H, RG_NONE, CN_NONE, 0 }, // 0x66 - LD H, [HL]
        { &s::MOV_RG_RG,    RG_H,    RG_A, CN_NONE, 0 }, // 0x67 - LD H, A
        { &s::MOV_RG_RG,    RG_L,    RG_B, CN_NONE, 0 }, // 0x68 - LD L, B
        { &s::MOV_RG_RG,    RG_L,    RG_C, CN_NONE, 0 }, // 0x69 - LD L, C
        { &s::MOV_RG_RG,    RG_L,    RG_D, CN_NONE, 0 }, // 0x6A - LD L, D
        { &s::MOV_RG_RG,    RG_L,    RG_E, CN_NONE, 0 }, // 0x6B - LD L, E
        { &s::MOV_RG_RG,    RG_L,    RG_H, CN_NONE, 0 }, // 0x6C - LD L, H
        { &s::MOV_RG_RG,    RG_L,    RG_L, CN_NONE, 0 }, // 0x6D - LD L, L
        { &s::MOV_RG_mRP,   RG_L, RG_NONE, CN_NONE, 0 }, // 0x6E - LD L, [HL]
        { &s::MOV_RG_RG,    RG_L,    RG_A, CN_NONE, 0 }, // 0x6F - LD L, A
        // 0x70 -> 0x7F
        { &s::MOV_mRP_RG,RG_NONE,    RG_B, CN_NONE, 0 }, // 0x70 - LD [HL], B
        { &s::MOV_mRP_RG,RG_NONE,    RG_C, CN_NONE, 0 }, // 0x71 - LD [HL], C
        { &s::MOV_mRP_RG,RG_NONE,    RG_D, CN_NONE, 0 }, // 0x72 - LD [HL], D
        { &s::MOV_mRP_RG,RG_NONE,    RG_E, CN_NONE, 0 }, // 0x73 - LD [HL], E
        { &s::MOV_mRP_RG,RG_NONE,    RG_H, CN_NONE, 0 }, // 0x74 - LD [HL], H
        { &s::MOV_mRP_RG,RG_NONE,    RG_L, CN_NONE, 0 }, // 0x75 - LD [HL], L
        { &s::HALT,      RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x76 - HALT
        { &s::MOV_mRP_RG,RG_NONE,    RG_A, CN_NONE, 0 }, // 0x77 - LD [HL], A
        { &s::MOV_RG_RG,    RG_A,    RG_B, CN_NONE, 0 }, // 0x78 - LD A, B
        { &s::MOV_RG_RG,    RG_A,    RG_C, CN_NONE, 0 }, // 0x79 - LD A, C
        { &s::MOV_RG_RG,    RG_A,    RG_D, CN_NONE, 0 }, // 0x7A - LD A, D
        { &s::MOV_RG_RG,    RG_A,    RG_E, CN_NONE, 0 }, // 0x7B - LD A, E
        { &s::MOV_RG_RG,    RG_A,    RG_H, CN_NONE, 0 }, // 0x7C - LD A, H
        { &s::MOV_RG_RG,    RG_A,    RG_L, CN_NONE, 0 }, // 0x7D - LD A, L
        { &s::MOV_RG_mRP,   RG_A, RG_NONE, CN_NONE, 0 }, // 0x7E - LD A, [HL]
        { &s::MOV_RG_RG,    RG_A,    RG_A, CN_NONE, 0 }, // 0x7F - LD A, A
        // 0x80 -> 0x8F
        { &s::ADD,       RG_NONE,    RG_B, CN_NONE, 0 }, // 0x80 - ADD A, B
        { &s::ADD,       RG_NONE,    RG_C, CN_NONE, 0 }, // 0x81 - ADD A, C
        { &s::ADD,       RG_NONE,    RG_D, CN_NONE, 0 }, // 0x82 - ADD A, D
        { &s::ADD,       RG_NONE,    RG_E, CN_NONE, 0 }, // 0x83 - ADD A, E
        { &s::ADD,       RG_NONE,    RG_H, CN_NONE, 0 }, // 0x84 - ADD A, H
        { &s::ADD,       RG_NONE,    RG_L, CN_NONE, 0 }, // 0x85 - ADD A, L
        { &s::ADD,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x86 - ADD A, [HL]
        { &s::ADD,       RG_NONE,    RG_A, CN_NONE, 0 }, // 0x87 - ADD A, A
        { &s::ADC,       RG_NONE,    RG_B, CN_NONE, 0 }, // 0x88 - ADC A, B
        { &s::ADC,       RG_NONE,    RG_C, CN_NONE, 0 }, // 0x89 - ADC A, C
        { &s::ADC,       RG_NONE,    RG_D, CN_NONE, 0 }, // 0x8A - ADC A, D
        { &s::ADC,       RG_NONE,    RG_E, CN_NONE, 0 }, // 0x8B - ADC A, E
        { &s::ADC,       RG_NONE,    RG_H, CN_NONE, 0 }, // 0x8C - ADC A, H
        { &s::ADC,       RG_NONE,    RG_L, CN_NONE, 0 }, // 0x8D - ADC A, L
        { &s::ADC,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x8E - ADC A, [HL]
        { &s::ADC,       RG_NONE,    RG_A, CN_NONE, 0 }, // 0x8F - ADC A, A
        // 0x90 -> 0x9F
        { &s::SUB,       RG_NONE,    RG_B, CN_NONE, 0 }, // 0x90 - SUB A, B
        { &s::SUB,       RG_NONE,    RG_C, CN_NONE, 0 }, // 0x91 - SUB A, C
        { &s::SUB,       RG_NONE,    RG_D, CN_NONE, 0 }, // 0x92 - SUB A, D
        { &s::SUB,       RG_NONE,    RG_E, CN_NONE, 0 }, // 0x93 - SUB A, E
        { &s::SUB,       RG_NONE,    RG_H, CN_NONE, 0 }, // 0x94 - SUB A, H
        { &s::SUB,       RG_NONE,    RG_L, CN_NONE, 0 }, // 0x95 - SUB A, L
        { &s::SUB,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x96 - SUB A, [HL]
        { &s::SUB,       RG_NONE,    RG_A, CN_NONE, 0 }, // 0x97 - SUB A, A
        { &s::SBC,       RG_NONE,    RG_B, CN_NONE, 0 }, // 0x98 - SBC A, B
        { &s::SBC,       RG_NONE,    RG_C, CN_NONE, 0 }, // 0x99 - SBC A, C
        { &s::SBC,       RG_NONE,    RG_D, CN_NONE, 0 }, // 0x9A - SBC A, D
        { &s::SBC,       RG_NONE,    RG_E, CN_NONE, 0 }, // 0x9B - SBC A, E
        { &s::SBC,       RG_NONE,    RG_H, CN_NONE, 0 }, // 0x9C - SBC A, H
        { &s::SBC,       RG_NONE,    RG_L, CN_NONE, 0 }, // 0x9D - SBC A, L
        { &s::SBC,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x9E - SBC A, [HL]
        { &s::SBC,       RG_NONE,    RG_A, CN_NONE, 0 }, // 0x9F - SBC A, A
        // 0xA0 -> 0xAF
        { &s::AND,       RG_NONE,    RG_B, CN_NONE, 0 }, // 0xA0 - AND A, B
        { &s::AND,       RG_NONE,    RG_C, CN_NONE, 0 }, // 0xA1 - AND A, C
        { &s::AND,       RG_NONE,    RG_D, CN_NONE, 0 }, // 0xA2 - AND A, D
        { &s::AND,       RG_NONE,    RG_E, CN_NONE, 0 }, // 0xA3 - AND A, E
        { &s::AND,       RG_NONE,    RG_H, CN_NONE, 0 }, // 0xA4 - AND A, H
        { &s::AND,       RG_NONE,    RG_L, CN_NONE, 0 }, // 0xA5 - AND A, L
        { &s::AND,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xA6 - AND A, [HL]
        { &s::AND,       RG_NONE,    RG_A, CN_NONE, 0 }, // 0xA7 - AND A, A
        { &s::XOR,       RG_NONE,    RG_B, CN_NONE, 0 }, // 0xA8 - XOR A, B
        { &s::XOR,       RG_NONE,    RG_C, CN_NONE, 0 }, // 0xA9 - XOR A, C
        { &s::XOR,       RG_NONE,    RG_D, CN_NONE, 0 }, // 0xAA - XOR A, D
        { &s::XOR,       RG_NONE,    RG_E, CN_NONE, 0 }, // 0xAB - XOR A, E
        { &s::XOR,       RG_NONE,    RG_H, CN_NONE, 0 }, // 0xAC - XOR A, H
        { &s::XOR,       RG_NONE,    RG_L, CN_NONE, 0 }, // 0xAD - XOR A, L
        { &s::XOR,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xAE - XOR A, [HL]
        { &s::XOR,       RG_NONE,    RG_A, CN_NONE, 0 }, // 0xAF - XOR A, A
        // 0xB0 -> 0xBF
        { &s::OR,        RG_NONE,    RG_B, CN_NONE, 0 }, // 0xB0 - OR A, B
        { &s::OR,        RG_NONE,    RG_C, CN_NONE, 0 }, // 0xB1 - OR A, C
        { &s::OR,        RG_NONE,    RG_D, CN_NONE, 0 }, // 0xB2 - OR A, D
        { &s::OR,        RG_NONE,    RG_E, CN_NONE, 0 }, // 0xB3 - OR A, E
        { &s::OR,        RG_NONE,    RG_H, CN_NONE, 0 }, // 0xB4 - OR A, H
        { &s::OR,        RG_NONE,    RG_L, CN_NONE, 0 }, // 0xB5 - OR A, L
        { &s::OR,        RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xB6 - OR A, [HL]
        { &s::OR,        RG_NONE,    RG_A, CN_NONE, 0 }, // 0xB7 - OR A, A
        { &s::CP,        RG_NONE,    RG_B, CN_NONE, 0 }, // 0xB0 - CP A, B
        { &s::CP,        RG_NONE,    RG_C, CN_NONE, 0 }, // 0xB1 - CP A, C
        { &s::CP,        RG_NONE,    RG_D, CN_NONE, 0 }, // 0xB2 - CP A, D
        { &s::CP,        RG_NONE,    RG_E, CN_NONE, 0 }, // 0xB3 - CP A, E
        { &s::CP,        RG_NONE,    RG_H, CN_NONE, 0 }, // 0xB4 - CP A, H
        { &s::CP,        RG_NONE,    RG_L, CN_NONE, 0 }, // 0xB5 - CP A, L
        { &s::CP,        RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xB6 - CP A, [HL]
        { &s::CP,        RG_NONE,    RG_A, CN_NONE, 0 }, // 0xB7 - CP A, A
        // 0xC0 -> 0xCF
        { &s::RET,       RG_NONE, RG_NONE,   CN_NZ, 0 }, // 0xC0 - RET NZ
        { &s::POP,         RP_BC, RG_NONE, CN_NONE, 0 }, // 0xC1 - POP BC
        { &s::JUMP,      RG_NONE, RG_NONE,   CN_NZ, 0 }, // 0xC2 - JP NZ, a16
        { &s::JUMP,      RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xC3 - JP a16
        { &s::CALL,      RG_NONE, RG_NONE,   CN_NZ, 0 }, // 0xC4 - CALL NZ, a16
        { &s::PUSH,      RG_NONE,   RP_BC, CN_NONE, 0 }, // 0xC5 - PUSH BC
        { &s::ADD,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xC6 - ADD A, d8
        { &s::RST,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xC7 - RST $0000
        { &s::RET,       RG_NONE, RG_NONE,    CN_Z, 0 }, // 0xC8 - RET Z
        { &s::RET,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xC9 - RET
        { &s::JUMP,      RG_NONE, RG_NONE,    CN_Z, 0 }, // 0xCA - JP Z, a16
        { &s::CB_OPCODES,RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xCB - CB Prefix Opcodes
        { &s::CALL,      RG_NONE, RG_NONE,    CN_Z, 0 }, // 0xCC - CALL Z, a16
        { &s::CALL,      RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xCD - CALL a16
        { &s::ADC,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xCE - ADC A, d8
        { &s::RST,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xCF - RST $0008
        // 0xD0 -> 0xDF
        { &s::RET,       RG_NONE, RG_NONE,   CN_NC, 0 }, // 0xD0 - RET NC, a16
        { &s::POP,         RP_DE, RG_NONE, CN_NONE, 0 }, // 0xD1 - POP DE
        { &s::JUMP,      RG_NONE, RG_NONE,   CN_NC, 0 }, // 0xD2 - JP NC, a16
        { &s::UNI,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xD3 - Invalid Opcode
        { &s::CALL,      RG_NONE, RG_NONE,   CN_NC, 0 }, // 0xD4 - CALL NC, a16
        { &s::PUSH,      RG_NONE,   RP_DE, CN_NONE, 0 }, // 0xD5 - PUSH DE
        { &s::SUB,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xD6 - SUB A, d8
        { &s::RST,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xD7 - RST $0010
        { &s::RET,       RG_NONE, RG_NONE,    CN_C, 0 }, // 0xD8 - RET C, a16
        { &s::RETI,      RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xD9 - RETI
        { &s::JUMP,      RG_NONE, RG_NONE,    CN_C, 0 }, // 0xDA - JP C, a16
        { &s::UNI,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xDB - Invalid Opcode
        { &s::CALL,      RG_NONE, RG_NONE,    CN_C, 0 }, // 0xD4 - CALL C, a16
        { &s::UNI,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xDD - Invalid Opcode
        { &s::SBC,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xDE - SBC A, d8
        { &s::RST,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xDF - RST $0018
        // 0xE0 -> 0xEF
        { &s::LDH,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xE0 - LDH [a8], A
        { &s::POP,         RP_HL, RG_NONE, CN_NONE, 0 }, // 0xE1 - POP HL
        { &s::LDH,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xE2 - LDH [C], A
        { &s::UNI,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xE3 - Invalid Opcode
        { &s::UNI,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xE4 - Invalid Opcode
        { &s::PUSH,      RG_NONE,   RP_HL, CN_NONE, 0 }, // 0xE5 - PUSH HL
        { &s::AND,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xE6 - AND A, d8
        { &s::RST,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xE7 - RST $0020
        { &s::ADD_SP_IMM,RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xE8 - ADD SP, d8
        { &s::JUMP,      RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xE9 - JP HL
        { &s::MOV_mRP_RG,RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xEA - LD [a16], A
        { &s::UNI,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xEB - Invalid Opcode
        { &s::UNI,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xEC - Invalid Opcode
        { &s::UNI,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xED - Invalid Opcode
        { &s::XOR,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xEE - XOR A, d8
        { &s::RST,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xEF - RST $0028
        // 0xF0 -> 0xFF
        { &s::LDH,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xF0 - LDH A, [a8]
        { &s::POP,         RP_AF, RG_NONE, CN_NONE, 0 }, // 0xF1 - POP AF
        { &s::LDH,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xF2 - LDH A, [C]
        { &s::DI,        RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xF3 - DI
        { &s::UNI,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xF4 - Invalid Opcode
        { &s::PUSH,      RG_NONE,   RP_AF, CN_NONE, 0 }, // 0xF5 - PUSH AF
        { &s::OR,        RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xF6 - OR A, d8
        { &s::RST,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xF7 - RST $0030
        { &s::ADD_SP_IMM,RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xF8 - LD HL, SP + d8
        { &s::LD_SP_HL,  RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xF9 - LD SP, HL
        { &s::MOV_RG_mRP,RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xFA - LD A, [a16]
        { &s::EI,        RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xFB - EI
        { &s::UNI,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xFC - Invalid Opcode
        { &s::UNI,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xFD - Invalid Opcode
        { &s::CP,        RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xFE - CP A, d8
        { &s::RST,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xFF - RST $0038
    };

    // Define CB-Indexed Opcodes
    cb_opcodes = {
        // 0x00 -> 0x0F
        { &s::RLC,          RG_B, RG_NONE, CN_NONE, 0 }, // 0x00 - RLC B
        { &s::RLC,          RG_C, RG_NONE, CN_NONE, 0 }, // 0x01 - RLC C
        { &s::RLC,          RG_D, RG_NONE, CN_NONE, 0 }, // 0x02 - RLC D
        { &s::RLC,          RG_E, RG_NONE, CN_NONE, 0 }, // 0x03 - RLC E
        { &s::RLC,          RG_H, RG_NONE, CN_NONE, 0 }, // 0x04 - RLC H
        { &s::RLC,          RG_L, RG_NONE, CN_NONE, 0 }, // 0x05 - RLC L
        { &s::RLC,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x06 - RLC [HL]
        { &s::RLC,          RG_A, RG_NONE, CN_NONE, 0 }, // 0x07 - RLC A
        { &s::RRC,          RG_B, RG_NONE, CN_NONE, 0 }, // 0x08 - RRC B
        { &s::RRC,          RG_C, RG_NONE, CN_NONE, 0 }, // 0x09 - RRC C
        { &s::RRC,          RG_D, RG_NONE, CN_NONE, 0 }, // 0x0A - RRC D
        { &s::RRC,          RG_E, RG_NONE, CN_NONE, 0 }, // 0x0B - RRC E
        { &s::RRC,          RG_H, RG_NONE, CN_NONE, 0 }, // 0x0C - RRC H
        { &s::RRC,          RG_L, RG_NONE, CN_NONE, 0 }, // 0x0D - RRC L
        { &s::RRC,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x0E - RRC [HL]
        { &s::RRC,          RG_A, RG_NONE, CN_NONE, 0 }, // 0x0F - RRC A
        // 0x10 -> 0x1F
        { &s::RL,           RG_B, RG_NONE, CN_NONE, 0 }, // 0x10 - RL B
        { &s::RL,           RG_C, RG_NONE, CN_NONE, 0 }, // 0x11 - RL C
        { &s::RL,           RG_D, RG_NONE, CN_NONE, 0 }, // 0x12 - RL D
        { &s::RL,           RG_E, RG_NONE, CN_NONE, 0 }, // 0x13 - RL E
        { &s::RL,           RG_H, RG_NONE, CN_NONE, 0 }, // 0x14 - RL H
        { &s::RL,           RG_L, RG_NONE, CN_NONE, 0 }, // 0x15 - RL L
        { &s::RL,        RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x16 - RL [HL]
        { &s::RL,           RG_A, RG_NONE, CN_NONE, 0 }, // 0x17 - RL A
        { &s::RR,           RG_B, RG_NONE, CN_NONE, 0 }, // 0x18 - RR B
        { &s::RR,           RG_C, RG_NONE, CN_NONE, 0 }, // 0x19 - RR C
        { &s::RR,           RG_D, RG_NONE, CN_NONE, 0 }, // 0x1A - RR D
        { &s::RR,           RG_E, RG_NONE, CN_NONE, 0 }, // 0x1B - RR E
        { &s::RR,           RG_H, RG_NONE, CN_NONE, 0 }, // 0x1C - RR H
        { &s::RR,           RG_L, RG_NONE, CN_NONE, 0 }, // 0x1D - RR L
        { &s::RR,        RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x1E - RR [HL]
        { &s::RR,           RG_A, RG_NONE, CN_NONE, 0 }, // 0x1F - RR A
        // 0x20 -> 0x2F
        { &s::SLA,          RG_B, RG_NONE, CN_NONE, 0 }, // 0x20 - SLA B
        { &s::SLA,          RG_C, RG_NONE, CN_NONE, 0 }, // 0x21 - SLA C
        { &s::SLA,          RG_D, RG_NONE, CN_NONE, 0 }, // 0x22 - SLA D
        { &s::SLA,          RG_E, RG_NONE, CN_NONE, 0 }, // 0x23 - SLA E
        { &s::SLA,          RG_H, RG_NONE, CN_NONE, 0 }, // 0x24 - SLA H
        { &s::SLA,          RG_L, RG_NONE, CN_NONE, 0 }, // 0x25 - SLA L
        { &s::SLA,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x26 - SLA [HL]
        { &s::SLA,          RG_A, RG_NONE, CN_NONE, 0 }, // 0x27 - SLA A
        { &s::SRA,          RG_B, RG_NONE, CN_NONE, 0 }, // 0x28 - SRA B
        { &s::SRA,          RG_C, RG_NONE, CN_NONE, 0 }, // 0x29 - SRA C
        { &s::SRA,          RG_D, RG_NONE, CN_NONE, 0 }, // 0x2A - SRA D
        { &s::SRA,          RG_E, RG_NONE, CN_NONE, 0 }, // 0x2B - SRA E
        { &s::SRA,          RG_H, RG_NONE, CN_NONE, 0 }, // 0x2C - SRA H
        { &s::SRA,          RG_L, RG_NONE, CN_NONE, 0 }, // 0x2D - SRA L
        { &s::SRA,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x2E - SRA [HL]
        { &s::SRA,          RG_A, RG_NONE, CN_NONE, 0 }, // 0x2F - SRA A
        // 0x30 -> 0x3F
        { &s::SWAP,         RG_B, RG_NONE, CN_NONE, 0 }, // 0x30 - SWAP B
        { &s::SWAP,         RG_C, RG_NONE, CN_NONE, 0 }, // 0x31 - SWAP C
        { &s::SWAP,         RG_D, RG_NONE, CN_NONE, 0 }, // 0x32 - SWAP D
        { &s::SWAP,         RG_E, RG_NONE, CN_NONE, 0 }, // 0x33 - SWAP E
        { &s::SWAP,         RG_H, RG_NONE, CN_NONE, 0 }, // 0x34 - SWAP H
        { &s::SWAP,         RG_L, RG_NONE, CN_NONE, 0 }, // 0x35 - SWAP L
        { &s::SWAP,      RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x36 - SWAP [HL]
        { &s::SWAP,         RG_A, RG_NONE, CN_NONE, 0 }, // 0x37 - SWAP A
        { &s::SRL,          RG_B, RG_NONE, CN_NONE, 0 }, // 0x38 - SRL B
        { &s::SRL,          RG_C, RG_NONE, CN_NONE, 0 }, // 0x39 - SRL C
        { &s::SRL,          RG_D, RG_NONE, CN_NONE, 0 }, // 0x3A - SRL D
        { &s::SRL,          RG_E, RG_NONE, CN_NONE, 0 }, // 0x3B - SRL E
        { &s::SRL,          RG_H, RG_NONE, CN_NONE, 0 }, // 0x3C - SRL H
        { &s::SRL,          RG_L, RG_NONE, CN_NONE, 0 }, // 0x3D - SRL L
        { &s::SRL,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x3E - SRL [HL]
        { &s::SRL,          RG_A, RG_NONE, CN_NONE, 0 }, // 0x3F - SRL A
        // 0x40 -> 0x4F
        { &s::BIT,          RG_B, RG_NONE, CN_NONE, 0 }, // 0x40 - BIT 0, B
        { &s::BIT,          RG_C, RG_NONE, CN_NONE, 0 }, // 0x41 - BIT 0, C
        { &s::BIT,          RG_D, RG_NONE, CN_NONE, 0 }, // 0x42 - BIT 0, D
        { &s::BIT,          RG_E, RG_NONE, CN_NONE, 0 }, // 0x43 - BIT 0, E
        { &s::BIT,          RG_H, RG_NONE, CN_NONE, 0 }, // 0x44 - BIT 0, H
        { &s::BIT,          RG_L, RG_NONE, CN_NONE, 0 }, // 0x45 - BIT 0, L
        { &s::BIT,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x46 - BIT 0, [HL]
        { &s::BIT,          RG_A, RG_NONE, CN_NONE, 0 }, // 0x47 - BIT 0, A
        { &s::BIT,          RG_B, RG_NONE, CN_NONE, 1 }, // 0x48 - BIT 1, B
        { &s::BIT,          RG_C, RG_NONE, CN_NONE, 1 }, // 0x49 - BIT 1, C
        { &s::BIT,          RG_D, RG_NONE, CN_NONE, 1 }, // 0x4A - BIT 1, D
        { &s::BIT,          RG_E, RG_NONE, CN_NONE, 1 }, // 0x4B - BIT 1, E
        { &s::BIT,          RG_H, RG_NONE, CN_NONE, 1 }, // 0x4C - BIT 1, H
        { &s::BIT,          RG_L, RG_NONE, CN_NONE, 1 }, // 0x4D - BIT 1, L
        { &s::BIT,       RG_NONE, RG_NONE, CN_NONE, 1 }, // 0x4E - BIT 1, [HL]
        { &s::BIT,          RG_A, RG_NONE, CN_NONE, 1 }, // 0x4F - BIT 1, A
        // 0x50 -> 0x5F
        { &s::BIT,          RG_B, RG_NONE, CN_NONE, 2 }, // 0x50 - BIT 2, B
        { &s::BIT,          RG_C, RG_NONE, CN_NONE, 2 }, // 0x51 - BIT 2, C
        { &s::BIT,          RG_D, RG_NONE, CN_NONE, 2 }, // 0x52 - BIT 2, D
        { &s::BIT,          RG_E, RG_NONE, CN_NONE, 2 }, // 0x53 - BIT 2, E
        { &s::BIT,          RG_H, RG_NONE, CN_NONE, 2 }, // 0x54 - BIT 2, H
        { &s::BIT,          RG_L, RG_NONE, CN_NONE, 2 }, // 0x55 - BIT 2, L
        { &s::BIT,       RG_NONE, RG_NONE, CN_NONE, 2 }, // 0x56 - BIT 2, [HL]
        { &s::BIT,          RG_A, RG_NONE, CN_NONE, 2 }, // 0x57 - BIT 2, A
        { &s::BIT,          RG_B, RG_NONE, CN_NONE, 3 }, // 0x58 - BIT 3, B
        { &s::BIT,          RG_C, RG_NONE, CN_NONE, 3 }, // 0x59 - BIT 3, C
        { &s::BIT,          RG_D, RG_NONE, CN_NONE, 3 }, // 0x5A - BIT 3, D
        { &s::BIT,          RG_E, RG_NONE, CN_NONE, 3 }, // 0x5B - BIT 3, E
        { &s::BIT,          RG_H, RG_NONE, CN_NONE, 3 }, // 0x5C - BIT 3, H
        { &s::BIT,          RG_L, RG_NONE, CN_NONE, 3 }, // 0x5D - BIT 3, L
        { &s::BIT,       RG_NONE, RG_NONE, CN_NONE, 3 }, // 0x5E - BIT 3, [HL]
        { &s::BIT,          RG_A, RG_NONE, CN_NONE, 3 }, // 0x5F - BIT 3, A
        // 0x60 -> 0x6F
        { &s::BIT,          RG_B, RG_NONE, CN_NONE, 4 }, // 0x60 - BIT 4, B
        { &s::BIT,          RG_C, RG_NONE, CN_NONE, 4 }, // 0x61 - BIT 4, C
        { &s::BIT,          RG_D, RG_NONE, CN_NONE, 4 }, // 0x62 - BIT 4, D
        { &s::BIT,          RG_E, RG_NONE, CN_NONE, 4 }, // 0x63 - BIT 4, E
        { &s::BIT,          RG_H, RG_NONE, CN_NONE, 4 }, // 0x64 - BIT 4, H
        { &s::BIT,          RG_L, RG_NONE, CN_NONE, 4 }, // 0x65 - BIT 4, L
        { &s::BIT,       RG_NONE, RG_NONE, CN_NONE, 4 }, // 0x66 - BIT 4, [HL]
        { &s::BIT,          RG_A, RG_NONE, CN_NONE, 4 }, // 0x67 - BIT 4, A
        { &s::BIT,          RG_B, RG_NONE, CN_NONE, 5 }, // 0x68 - BIT 5, B
        { &s::BIT,          RG_C, RG_NONE, CN_NONE, 5 }, // 0x69 - BIT 5, C
        { &s::BIT,          RG_D, RG_NONE, CN_NONE, 5 }, // 0x6A - BIT 5, D
        { &s::BIT,          RG_E, RG_NONE, CN_NONE, 5 }, // 0x6B - BIT 5, E
        { &s::BIT,          RG_H, RG_NONE, CN_NONE, 5 }, // 0x6C - BIT 5, H
        { &s::BIT,          RG_L, RG_NONE, CN_NONE, 5 }, // 0x6D - BIT 5, L
        { &s::BIT,       RG_NONE, RG_NONE, CN_NONE, 5 }, // 0x6E - BIT 5, [HL]
        { &s::BIT,          RG_A, RG_NONE, CN_NONE, 5 }, // 0x6F - BIT 5, A
        // 0x70 -> 0x7F  
        { &s::BIT,          RG_B, RG_NONE, CN_NONE, 6 }, // 0x70 - BIT 6, B
        { &s::BIT,          RG_C, RG_NONE, CN_NONE, 6 }, // 0x71 - BIT 6, C
        { &s::BIT,          RG_D, RG_NONE, CN_NONE, 6 }, // 0x72 - BIT 6, D
        { &s::BIT,          RG_E, RG_NONE, CN_NONE, 6 }, // 0x73 - BIT 6, E
        { &s::BIT,          RG_H, RG_NONE, CN_NONE, 6 }, // 0x74 - BIT 6, H
        { &s::BIT,          RG_L, RG_NONE, CN_NONE, 6 }, // 0x75 - BIT 6, L
        { &s::BIT,       RG_NONE, RG_NONE, CN_NONE, 6 }, // 0x76 - BIT 6, [HL]
        { &s::BIT,          RG_A, RG_NONE, CN_NONE, 6 }, // 0x77 - BIT 6, A
        { &s::BIT,          RG_B, RG_NONE, CN_NONE, 7 }, // 0x78 - BIT 7, B
        { &s::BIT,          RG_C, RG_NONE, CN_NONE, 7 }, // 0x79 - BIT 7, C
        { &s::BIT,          RG_D, RG_NONE, CN_NONE, 7 }, // 0x7A - BIT 7, D
        { &s::BIT,          RG_E, RG_NONE, CN_NONE, 7 }, // 0x7B - BIT 7, E
        { &s::BIT,          RG_H, RG_NONE, CN_NONE, 7 }, // 0x7C - BIT 7, H
        { &s::BIT,          RG_L, RG_NONE, CN_NONE, 7 }, // 0x7D - BIT 7, L
        { &s::BIT,       RG_NONE, RG_NONE, CN_NONE, 7 }, // 0x7E - BIT 7, [HL]
        { &s::BIT,          RG_A, RG_NONE, CN_NONE, 7 }, // 0x7F - BIT 7, A
        // 0x80 -> 0x8F
        { &s::RES,          RG_B, RG_NONE, CN_NONE, 0 }, // 0x80 - RES 0, B
        { &s::RES,          RG_C, RG_NONE, CN_NONE, 0 }, // 0x81 - RES 0, C
        { &s::RES,          RG_D, RG_NONE, CN_NONE, 0 }, // 0x82 - RES 0, D
        { &s::RES,          RG_E, RG_NONE, CN_NONE, 0 }, // 0x83 - RES 0, E
        { &s::RES,          RG_H, RG_NONE, CN_NONE, 0 }, // 0x84 - RES 0, H
        { &s::RES,          RG_L, RG_NONE, CN_NONE, 0 }, // 0x85 - RES 0, L
        { &s::RES,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0x86 - RES 0, [HL]
        { &s::RES,          RG_A, RG_NONE, CN_NONE, 0 }, // 0x87 - RES 0, A
        { &s::RES,          RG_B, RG_NONE, CN_NONE, 1 }, // 0x88 - RES 1, B
        { &s::RES,          RG_C, RG_NONE, CN_NONE, 1 }, // 0x89 - RES 1, C
        { &s::RES,          RG_D, RG_NONE, CN_NONE, 1 }, // 0x8A - RES 1, D
        { &s::RES,          RG_E, RG_NONE, CN_NONE, 1 }, // 0x8B - RES 1, E
        { &s::RES,          RG_H, RG_NONE, CN_NONE, 1 }, // 0x8C - RES 1, H
        { &s::RES,          RG_L, RG_NONE, CN_NONE, 1 }, // 0x8D - RES 1, L
        { &s::RES,       RG_NONE, RG_NONE, CN_NONE, 1 }, // 0x8E - RES 1, [HL]
        { &s::RES,          RG_A, RG_NONE, CN_NONE, 1 }, // 0x8F - RES 1, A
        // 0x90 -> 0x9F
        { &s::RES,          RG_B, RG_NONE, CN_NONE, 2 }, // 0x90 - RES 2, B
        { &s::RES,          RG_C, RG_NONE, CN_NONE, 2 }, // 0x91 - RES 2, C
        { &s::RES,          RG_D, RG_NONE, CN_NONE, 2 }, // 0x92 - RES 2, D
        { &s::RES,          RG_E, RG_NONE, CN_NONE, 2 }, // 0x93 - RES 2, E
        { &s::RES,          RG_H, RG_NONE, CN_NONE, 2 }, // 0x94 - RES 2, H
        { &s::RES,          RG_L, RG_NONE, CN_NONE, 2 }, // 0x95 - RES 2, L
        { &s::RES,       RG_NONE, RG_NONE, CN_NONE, 2 }, // 0x96 - RES 2, [HL]
        { &s::RES,          RG_A, RG_NONE, CN_NONE, 2 }, // 0x97 - RES 2, A
        { &s::RES,          RG_B, RG_NONE, CN_NONE, 3 }, // 0x98 - RES 3, B
        { &s::RES,          RG_C, RG_NONE, CN_NONE, 3 }, // 0x99 - RES 3, C
        { &s::RES,          RG_D, RG_NONE, CN_NONE, 3 }, // 0x9A - RES 3, D
        { &s::RES,          RG_E, RG_NONE, CN_NONE, 3 }, // 0x9B - RES 3, E
        { &s::RES,          RG_H, RG_NONE, CN_NONE, 3 }, // 0x9C - RES 3, H
        { &s::RES,          RG_L, RG_NONE, CN_NONE, 3 }, // 0x9D - RES 3, L
        { &s::RES,       RG_NONE, RG_NONE, CN_NONE, 3 }, // 0x9E - RES 3, [HL]
        { &s::RES,          RG_A, RG_NONE, CN_NONE, 3 }, // 0x9F - RES 3, A
        // 0xA0 -> 0xAF
        { &s::RES,          RG_B, RG_NONE, CN_NONE, 4 }, // 0xA0 - RES 4, B
        { &s::RES,          RG_C, RG_NONE, CN_NONE, 4 }, // 0xA1 - RES 4, C
        { &s::RES,          RG_D, RG_NONE, CN_NONE, 4 }, // 0xA2 - RES 4, D
        { &s::RES,          RG_E, RG_NONE, CN_NONE, 4 }, // 0xA3 - RES 4, E
        { &s::RES,          RG_H, RG_NONE, CN_NONE, 4 }, // 0xA4 - RES 4, H
        { &s::RES,          RG_L, RG_NONE, CN_NONE, 4 }, // 0xA5 - RES 4, L
        { &s::RES,       RG_NONE, RG_NONE, CN_NONE, 4 }, // 0xA6 - RES 4, [HL]
        { &s::RES,          RG_A, RG_NONE, CN_NONE, 4 }, // 0xA7 - RES 4, A
        { &s::RES,          RG_B, RG_NONE, CN_NONE, 5 }, // 0xA8 - RES 5, B
        { &s::RES,          RG_C, RG_NONE, CN_NONE, 5 }, // 0xA9 - RES 5, C
        { &s::RES,          RG_D, RG_NONE, CN_NONE, 5 }, // 0xAA - RES 5, D
        { &s::RES,          RG_E, RG_NONE, CN_NONE, 5 }, // 0xAB - RES 5, E
        { &s::RES,          RG_H, RG_NONE, CN_NONE, 5 }, // 0xAC - RES 5, H
        { &s::RES,          RG_L, RG_NONE, CN_NONE, 5 }, // 0xAD - RES 5, L
        { &s::RES,       RG_NONE, RG_NONE, CN_NONE, 5 }, // 0xAE - RES 5, [HL]
        { &s::RES,          RG_A, RG_NONE, CN_NONE, 5 }, // 0xAF - RES 5, A
        // 0xB0 -> 0xBF  
        { &s::RES,          RG_B, RG_NONE, CN_NONE, 6 }, // 0xB0 - RES 6, B
        { &s::RES,          RG_C, RG_NONE, CN_NONE, 6 }, // 0xB1 - RES 6, C
        { &s::RES,          RG_D, RG_NONE, CN_NONE, 6 }, // 0xB2 - RES 6, D
        { &s::RES,          RG_E, RG_NONE, CN_NONE, 6 }, // 0xB3 - RES 6, E
        { &s::RES,          RG_H, RG_NONE, CN_NONE, 6 }, // 0xB4 - RES 6, H
        { &s::RES,          RG_L, RG_NONE, CN_NONE, 6 }, // 0xB5 - RES 6, L
        { &s::RES,       RG_NONE, RG_NONE, CN_NONE, 6 }, // 0xB6 - RES 6, [HL]
        { &s::RES,          RG_A, RG_NONE, CN_NONE, 6 }, // 0xB7 - RES 6, A
        { &s::RES,          RG_B, RG_NONE, CN_NONE, 7 }, // 0xB8 - RES 7, B
        { &s::RES,          RG_C, RG_NONE, CN_NONE, 7 }, // 0xB9 - RES 7, C
        { &s::RES,          RG_D, RG_NONE, CN_NONE, 7 }, // 0xBA - RES 7, D
        { &s::RES,          RG_E, RG_NONE, CN_NONE, 7 }, // 0xBB - RES 7, E
        { &s::RES,          RG_H, RG_NONE, CN_NONE, 7 }, // 0xBC - RES 7, H
        { &s::RES,          RG_L, RG_NONE, CN_NONE, 7 }, // 0xBD - RES 7, L
        { &s::RES,       RG_NONE, RG_NONE, CN_NONE, 7 }, // 0xBE - RES 7, [HL]
        { &s::RES,          RG_A, RG_NONE, CN_NONE, 7 }, // 0xBF - RES 7, A
        // 0xC0 -> 0xCF
        { &s::SET,          RG_B, RG_NONE, CN_NONE, 0 }, // 0xC0 - SET 0, B
        { &s::SET,          RG_C, RG_NONE, CN_NONE, 0 }, // 0xC1 - SET 0, C
        { &s::SET,          RG_D, RG_NONE, CN_NONE, 0 }, // 0xC2 - SET 0, D
        { &s::SET,          RG_E, RG_NONE, CN_NONE, 0 }, // 0xC3 - SET 0, E
        { &s::SET,          RG_H, RG_NONE, CN_NONE, 0 }, // 0xC4 - SET 0, H
        { &s::SET,          RG_L, RG_NONE, CN_NONE, 0 }, // 0xC5 - SET 0, L
        { &s::SET,       RG_NONE, RG_NONE, CN_NONE, 0 }, // 0xC6 - SET 0, [HL]
        { &s::SET,          RG_A, RG_NONE, CN_NONE, 0 }, // 0xC7 - SET 0, A
        { &s::SET,          RG_B, RG_NONE, CN_NONE, 1 }, // 0xC8 - SET 1, B
        { &s::SET,          RG_C, RG_NONE, CN_NONE, 1 }, // 0xC9 - SET 1, C
        { &s::SET,          RG_D, RG_NONE, CN_NONE, 1 }, // 0xCA - SET 1, D
        { &s::SET,          RG_E, RG_NONE, CN_NONE, 1 }, // 0xCB - SET 1, E
        { &s::SET,          RG_H, RG_NONE, CN_NONE, 1 }, // 0xCC - SET 1, H
        { &s::SET,          RG_L, RG_NONE, CN_NONE, 1 }, // 0xCD - SET 1, L
        { &s::SET,       RG_NONE, RG_NONE, CN_NONE, 1 }, // 0xCE - SET 1, [HL]
        { &s::SET,          RG_A, RG_NONE, CN_NONE, 1 }, // 0xCF - SET 1, A
        // 0xD0 -> 0xDF
        { &s::SET,          RG_B, RG_NONE, CN_NONE, 2 }, // 0xD0 - SET 2, B
        { &s::SET,          RG_C, RG_NONE, CN_NONE, 2 }, // 0xD1 - SET 2, C
        { &s::SET,          RG_D, RG_NONE, CN_NONE, 2 }, // 0xD2 - SET 2, D
        { &s::SET,          RG_E, RG_NONE, CN_NONE, 2 }, // 0xD3 - SET 2, E
        { &s::SET,          RG_H, RG_NONE, CN_NONE, 2 }, // 0xD4 - SET 2, H
        { &s::SET,          RG_L, RG_NONE, CN_NONE, 2 }, // 0xD5 - SET 2, L
        { &s::SET,       RG_NONE, RG_NONE, CN_NONE, 2 }, // 0xD6 - SET 2, [HL]
        { &s::SET,          RG_A, RG_NONE, CN_NONE, 2 }, // 0xD7 - SET 2, A
        { &s::SET,          RG_B, RG_NONE, CN_NONE, 3 }, // 0xD8 - SET 3, B
        { &s::SET,          RG_C, RG_NONE, CN_NONE, 3 }, // 0xD9 - SET 3, C
        { &s::SET,          RG_D, RG_NONE, CN_NONE, 3 }, // 0xDA - SET 3, D
        { &s::SET,          RG_E, RG_NONE, CN_NONE, 3 }, // 0xDB - SET 3, E
        { &s::SET,          RG_H, RG_NONE, CN_NONE, 3 }, // 0xDC - SET 3, H
        { &s::SET,          RG_L, RG_NONE, CN_NONE, 3 }, // 0xDD - SET 3, L
        { &s::SET,       RG_NONE, RG_NONE, CN_NONE, 3 }, // 0xDE - SET 3, [HL]
        { &s::SET,          RG_A, RG_NONE, CN_NONE, 3 }, // 0xDF - SET 3, A
        // 0xE0 -> 0xEF
        { &s::SET,          RG_B, RG_NONE, CN_NONE, 4 }, // 0xE0 - SET 4, B
        { &s::SET,          RG_C, RG_NONE, CN_NONE, 4 }, // 0xE1 - SET 4, C
        { &s::SET,          RG_D, RG_NONE, CN_NONE, 4 }, // 0xE2 - SET 4, D
        { &s::SET,          RG_E, RG_NONE, CN_NONE, 4 }, // 0xE3 - SET 4, E
        { &s::SET,          RG_H, RG_NONE, CN_NONE, 4 }, // 0xE4 - SET 4, H
        { &s::SET,          RG_L, RG_NONE, CN_NONE, 4 }, // 0xE5 - SET 4, L
        { &s::SET,       RG_NONE, RG_NONE, CN_NONE, 4 }, // 0xE6 - SET 4, [HL]
        { &s::SET,          RG_A, RG_NONE, CN_NONE, 4 }, // 0xE7 - SET 4, A
        { &s::SET,          RG_B, RG_NONE, CN_NONE, 5 }, // 0xE8 - SET 5, B
        { &s::SET,          RG_C, RG_NONE, CN_NONE, 5 }, // 0xE9 - SET 5, C
        { &s::SET,          RG_D, RG_NONE, CN_NONE, 5 }, // 0xEA - SET 5, D
        { &s::SET,          RG_E, RG_NONE, CN_NONE, 5 }, // 0xEB - SET 5, E
        { &s::SET,          RG_H, RG_NONE, CN_NONE, 5 }, // 0xEC - SET 5, H
        { &s::SET,          RG_L, RG_NONE, CN_NONE, 5 }, // 0xED - SET 5, L
        { &s::SET,       RG_NONE, RG_NONE, CN_NONE, 5 }, // 0xEE - SET 5, [HL]
        { &s::SET,          RG_A, RG_NONE, CN_NONE, 5 }, // 0xEF - SET 5, A
        // 0xF0 -> 0xFF  
        { &s::SET,          RG_B, RG_NONE, CN_NONE, 6 }, // 0xF0 - SET 6, B
        { &s::SET,          RG_C, RG_NONE, CN_NONE, 6 }, // 0xF1 - SET 6, C
        { &s::SET,          RG_D, RG_NONE, CN_NONE, 6 }, // 0xF2 - SET 6, D
        { &s::SET,          RG_E, RG_NONE, CN_NONE, 6 }, // 0xF3 - SET 6, E
        { &s::SET,          RG_H, RG_NONE, CN_NONE, 6 }, // 0xF4 - SET 6, H
        { &s::SET,          RG_L, RG_NONE, CN_NONE, 6 }, // 0xF5 - SET 6, L
        { &s::SET,       RG_NONE, RG_NONE, CN_NONE, 6 }, // 0xF6 - SET 6, [HL]
        { &s::SET,          RG_A, RG_NONE, CN_NONE, 6 }, // 0xF7 - SET 6, A
        { &s::SET,          RG_B, RG_NONE, CN_NONE, 7 }, // 0xF8 - SET 7, B
        { &s::SET,          RG_C, RG_NONE, CN_NONE, 7 }, // 0xF9 - SET 7, C
        { &s::SET,          RG_D, RG_NONE, CN_NONE, 7 }, // 0xFA - SET 7, D
        { &s::SET,          RG_E, RG_NONE, CN_NONE, 7 }, // 0xFB - SET 7, E
        { &s::SET,          RG_H, RG_NONE, CN_NONE, 7 }, // 0xFC - SET 7, H
        { &s::SET,          RG_L, RG_NONE, CN_NONE, 7 }, // 0xFD - SET 7, L
        { &s::SET,       RG_NONE, RG_NONE, CN_NONE, 7 }, // 0xFE - SET 7, [HL]
        { &s::SET,          RG_A, RG_NONE, CN_NONE, 7 }, // 0xFF - SET 7, A
    };
}

SM83::~SM83( void ) {
    // Nothing to do
}

/* Memory */
uint8_t SM83::read_byte( uint16_t addr ) {
    return bus->bus_read( addr );
}

uint16_t SM83::read_word( uint16_t addr ) {
    uint8_t lo = read_byte( addr );
    uint8_t hi = read_byte( addr + 1);
    return ( hi << 8 ) | lo;
}

uint8_t SM83::fetch_byte( void ) {
    uint8_t data = read_byte( PC );
    PC++;
    return data;
}

uint16_t SM83::fetch_word( void ) {
    uint8_t lo = fetch_byte();
    uint8_t hi = fetch_byte();
    return ( hi << 8 ) | lo;
}

void SM83::write_byte( uint16_t addr,  uint8_t data ) {
    bus->bus_write( addr, data );
}

void SM83::write_word( uint16_t addr, uint16_t data ) {
    uint8_t lo = data & 0xFF;
    uint8_t hi = data >> 8;

    write_byte(     addr, lo );
    write_byte( addr + 1, hi );
}

/* Flags */
void SM83::set_flag( FLAG f, bool val ) {
    if ( val ) {
		F |= f;
	}else {
		F &= ~f;
	}
}

bool SM83::get_flag( FLAG f ) {
    return F & f;
}

bool SM83::will_carry( uint8_t bit_no, uint16_t a, uint16_t b, bool cy ) {
	uint32_t result = a + b;
	if ( cy ) {
		result++;
	}

	uint16_t carry = result ^ a ^ b;

	if ( carry & ( 1 << bit_no ) ) {
		return true;
	}

	return false;
}

/* Registers */
uint16_t SM83::get_register( REG r ) {
    uint16_t ret = 0x0000;
    switch( r ) {
        // Registers
        case  RG_A: ret = A; break;
        case  RG_F: ret = F; break;
        case  RG_B: ret = B; break;
        case  RG_C: ret = C; break;
        case  RG_D: ret = D; break;
        case  RG_E: ret = E; break;
        case  RG_H: ret = H; break;
        case  RG_L: ret = L; break;
        // Register Pairs
        case RP_AF: ret = ( A << 8 ) | F; break;
        case RP_BC: ret = ( B << 8 ) | C; break;
        case RP_DE: ret = ( D << 8 ) | E; break;
        case RP_HL: ret = ( H << 8 ) | L; break;
        case RP_SP: ret = SP; break;
        default: UNI(); break;
    }

    return ret;
}

void SM83::set_register( REG r, uint16_t val ) {
        switch( r ) {
        // Registers
        case  RG_A: A = val & 0xFF; break;
        case  RG_F: F = val & 0xFF; F |= 0x02; F &= ~( 0x0F ); break;
        case  RG_B: B = val & 0xFF; break;
        case  RG_C: C = val & 0xFF; break;
        case  RG_D: D = val & 0xFF; break;
        case  RG_E: E = val & 0xFF; break;
        case  RG_H: H = val & 0xFF; break;
        case  RG_L: L = val & 0xFF; break;
        // Register Pairs
        case RP_AF: A = ( val >> 8 ); F = val & 0xFF; F |= 0x02; F &= ~( 0x0F ); break;
        case RP_BC: B = ( val >> 8 ); C = val & 0xFF; break;
        case RP_DE: D = ( val >> 8 ); E = val & 0xFF; break;
        case RP_HL: H = ( val >> 8 ); L = val & 0xFF; break;
        case RP_SP: SP = val; break;
        default: UNI(); break;
    }
}

/* Conditionals */
bool SM83::parse_conditional( COND cond ) {
    bool ret = false;

    switch( cond ) {
        case CN_NONE: ret = true; break;
        case    CN_Z: ret =  get_flag( F_Z ); break;
        case   CN_NZ: ret = !get_flag( F_Z ); break;
        case    CN_H: ret =  get_flag( F_H ); break;
        case   CN_NH: ret = !get_flag( F_H ); break;
        case    CN_N: ret =  get_flag( F_N ); break;
        case   CN_NN: ret = !get_flag( F_N ); break;
        case    CN_C: ret =  get_flag( F_C ); break;
        case   CN_NC: ret = !get_flag( F_C ); break;
        default: UNI(); break;
    }

    return ret;
}

/* Emulation */
void SM83::clock(void) {

    // Only advance if the previous instruction is done
    if (cycles != 0) {
        cycles--;
        return;
    }

    // Logging for GB Doctor
    DBG();

    // Fetch next opcode
    opcode = fetch_byte();

    // Execute opcode
    (this->*opcodes[opcode].operate)();

    // If IE was called
    if (ei_pending) {
        // Delay the interrupt by one instruction
        if (interrupt_enabled_delay) {
            interrupt_enabled = true;
            interrupt_enabled_delay = false;
            ei_pending = false;
        } else {
            interrupt_enabled_delay = true;
        }
    }
    
    // After an instruction has been executed and interrupts are enabled
    if (interrupt_enabled) {

        // Fetch waiting interrupts
        uint8_t pending =
            read_byte(IE_ADDRESS) &
            read_byte(IF) &
            0x1F;

        // If there are any, deploy them
        if (pending) {
            parse_interrupts(pending);
        }
    }
}

void SM83::parse_interrupts( uint8_t interrupts_pending ) {
    // If an interrupt is pending, and cpu is halted, unhalt it
    if( halted ) {
        halted = false;
    }

    // Interrupts pending is non-zero, so there IS an interrupt pending:
    // Push PC to stack
    interrupt_enabled = false;
    push_to_stack( PC );
    cycles += 20;

    // Check VBLANK interrupt
    if( interrupts_pending & IT_VBLANK ) {
        bus->clear_interrupt( IT_VBLANK );
        PC = IV_VBLANK;
        return;
    }

    // Check LCDSTAT interrupt
    else if( interrupts_pending & IT_LCD_STAT ) {
        bus->clear_interrupt( IT_LCD_STAT );
        PC = IV_LCD_STAT;
        return;
    }
    
    // Check TIMER interrupt
    else if( interrupts_pending & IT_TIMER ) {
        bus->clear_interrupt( IT_TIMER );
        PC = IV_TIMER;
        return;
    }

    // Check SERIAL interrupt
    else if( interrupts_pending & IT_SERIAL ) {
        bus->clear_interrupt( IT_SERIAL );
        PC = IV_SERIAL;
        return;
    }

    // Check JOYPAD interrupt
    else if( interrupts_pending & IT_JOYPAD ) {
        bus->clear_interrupt( IT_JOYPAD );
        PC = IV_JOYPAD;
        return;
    }

}

void SM83::reset( void ) {
    A = 0x01;
    F = 0xB0;
    B = 0x00;
    C = 0x13;
    D = 0x00;
    E = 0xD8;
    H = 0x01;
    L = 0x4D;

    PC = 0x0100;
    SP = 0xFFFE;

    halted = false;
    error = false;
    interrupt_enabled = false;
    opcode = 0x00;
    cycles = 0;
}

/* Utility */
uint8_t SM83::internal_add( uint8_t a, uint8_t b, bool cy ) {
    uint8_t res = a + b;
	if ( cy ) {
		res++;
	}

    set_flag( F_Z, res == 0x00 );
    set_flag( F_N, res & 0x80 );
    set_flag( F_H, will_carry( 4, a, b, cy ) );
    set_flag( F_C, will_carry( 8, a, b, cy ) );
	
	return res;
}

uint16_t SM83::pop_from_stack( void ) {
    uint16_t ret = read_word( SP );
    SP += 2;
    return ret;
}

void SM83::push_to_stack( uint16_t data ) {
    SP -= 2;
    write_word( SP, data );
}

void SM83::DBG( void ) {
    //A:00 F:11 B:22 C:33 D:44 E:55 H:66 L:77 SP:8888 PC:9999 PCMEM:AA,BB,CC,DD
    printf( 
        "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
        A, F, B, C, D, E, H, L, SP, PC,
        read_byte( PC ), read_byte( PC + 1 ), read_byte( PC + 2 ), read_byte( PC + 3 )
    );
    /*
    printf( "PC: %04X, AF: %04X, BC: %04X, DE: %04X, HL: %04X, SP: %04X, CYC: %d",
					PC, get_register( RP_AF ), get_register( RP_BC ), get_register( RP_DE ), get_register( RP_HL ),
					SP, cycles );

	printf( "\t(%02X %02X %02X %02X)", read_byte( PC ), read_byte( PC + 1 ),
					read_byte( PC + 2 ), read_byte( PC + 3 ) );

	printf( "\n" );
    */
}

/* Opcodes */
void SM83::UNI( void ) {
    halted = true;
    error = true;
    cycles = 1;
}

void SM83::NOP( void ) {
    // Do nothing for 4 cycles
    cycles = 4;
}

void SM83::LD_RP_IMM( void ) {
    set_register( GET_DST, fetch_word() );
    cycles = 12;
}

void SM83::LD_mRP_A( void ) {
    write_byte( get_register( GET_DST ), A );
    if( opcode == 0x22 ) {
        set_register( RP_HL, get_register( RP_HL ) + 1 );
    }else if( opcode == 0x32 ) {
        set_register( RP_HL, get_register( RP_HL ) - 1 );
    }
    cycles = 12;
}

void SM83::INC_RP( void ) {
    set_register(
        GET_DST,
        get_register( GET_DST ) + 1
    );

    cycles = 8;
}

void SM83::INC_RG( void ) {
    uint8_t result = 0x00;

    if( opcode == 0x34 ) {
        result = read_byte( get_register( RP_HL ) ) + 1;
        write_byte( get_register( RP_HL ), result );
        cycles = 8;
    }else {
        result = get_register( GET_DST ) + 1;
        set_register( GET_DST, result );
        cycles = 8;
    }

    // Set flags
    set_flag( F_Z, result == 0x00 );
    set_flag( F_N, false );
    set_flag( F_H, (result & 0x0F ) == 0x00 );
}

void SM83::DEC_RG( void ) {
    uint8_t result = 0x00;

    if( opcode == 0x35 ) {
        result = read_byte( get_register( RP_HL ) ) - 1;
        write_byte( get_register( RP_HL ), result );
        cycles = 8;
    }else {
        result = get_register( GET_DST ) - 1;
        set_register( GET_DST, result );
        cycles = 8;
    }

    // Set flags
    set_flag( F_Z, result == 0x00 );
    set_flag( F_N, true );
    set_flag( F_H, (result & 0x0F ) == 0x0F );

}

void SM83::LD_RG_IMM( void ) {
    if( opcode == 0x36 ) {
        write_byte( get_register( RP_HL ), fetch_byte() );
        cycles = 12;
    }else {
        set_register( GET_DST, fetch_byte() );
        cycles = 8;
    }
}

void SM83::RLCA( void ) {
    bool old_MSB = A & 0x80;

    A <<= 1;

    if( old_MSB ) {
        A |= 0x01;
    }

    set_flag( F_Z, false );
    set_flag( F_N, false );
    set_flag( F_H, false );
    set_flag( F_C, old_MSB );

    cycles = 4;
}


void SM83::LD_mRP_SP( void ) {
    write_word( fetch_word(), SP );
    cycles = 20;
}

void SM83::ADD_HL_RP( void ) {
    uint32_t result = get_register( RP_HL ) + get_register( GET_SRC );

    set_flag( F_N, false );
    set_flag( F_H, will_carry( 12, get_register( RP_HL ), get_register( GET_SRC ), false ) );
    set_flag( F_C, result > 0xFFFF );

    set_register( RP_HL, result & 0xFFFF );
    cycles = 8;
}

void SM83::LD_A_mRP( void ) {
    A = read_byte( get_register(GET_SRC) );

    if( opcode == 0x2A ) {
        set_register( RP_HL, get_register( RP_HL ) + 1 );
    }else if( opcode == 0x3A ) {
        set_register( RP_HL, get_register( RP_HL ) - 1 );
    }

    cycles = 8;
}

void SM83::DEC_RP( void ) {
    set_register(
        GET_DST,
        get_register( GET_DST ) - 1
    );

    cycles = 8;
}

void SM83::RRCA( void ) {
    bool old_LSB = A & 0x01;

    A >>= 1;

    if( old_LSB ) {
        A |= 0x80;
    }

    set_flag( F_Z, false );
    set_flag( F_N, false );
    set_flag( F_H, false );
    set_flag( F_C, old_LSB );

    cycles = 4;
}

void SM83::STOP( void ) {
    PC++;
    halted = true;
    cycles = 4;
}


void SM83::RLA( void ) {
    bool old_cy = get_flag( F_C );

    set_flag( F_Z, false );
    set_flag( F_N, false );
    set_flag( F_H, false );
    set_flag( F_C, A & 0x80 );

    A <<= 1;

    if( old_cy ) {
        A |= 0x01;
    }

    cycles = 4;
}

void SM83::JUMP_REL( void ) {
    // Fetch a byte and convert it to signed integer
    int8_t offset = (int8_t)fetch_byte();

    if( parse_conditional( GET_COND ) ) {
        PC += offset;
        cycles = 12;
    }else {
        cycles = 8;
    }

}

void SM83::RRA( void ) {
    bool old_cy = get_flag( F_C );

    set_flag( F_Z, false );
    set_flag( F_N, false );
    set_flag( F_H, false );
    set_flag( F_C, A & 0x01 );

    A >>= 1;

    if( old_cy ) {
        A |= 0x80;
    }

    cycles = 4;
}

void SM83::DAA( void ) {

    if ( get_flag( F_N ) ) {
        if ( get_flag( F_H ) ) {
            A += 0xFA;
        }
        if ( get_flag( F_C ) ) {
            A += 0xA0;
        }
    } else {
        uint16_t tmp_word = A;

        if ( ( ( A & 0x0F ) > 9 ) || get_flag( F_H ) ) {
            tmp_word += 0x06;
        }

        if ( ( ( tmp_word & 0x1F0 ) > 0x90 ) || get_flag( F_C ) ) {
            tmp_word += 0x60;
            set_flag( F_C, true );
        } else {
            set_flag( F_C, false );
        }

        A = uint8_t( tmp_word );
    }

    set_flag( F_Z, A == 0x00 );
    set_flag( F_H, false );

     cycles = 4;
}

void SM83::CPL( void ) {
    A = ~A;

    set_flag( F_N, true );
    set_flag( F_H, true );
    
    cycles = 4;
}

void SM83::SCF( void ) {
    set_flag( F_N, false );
    set_flag( F_H, false );
    set_flag( F_C,  true );
    cycles = 4;
}

void SM83::CCF( void ) {
    set_flag( F_N, false );
    set_flag( F_H, false );
    set_flag( F_C, !get_flag( F_C ) );
    cycles = 4;
}

void SM83::MOV_RG_RG( void ) {
    set_register( GET_DST, get_register( GET_SRC ) );
    cycles = 4;
}

void SM83::MOV_RG_mRP( void ) {
    // Special Case!
    if( opcode == 0xFA ) {
        A = read_byte( fetch_word() );
        cycles = 16;
        return;
    }

    set_register( GET_DST, read_byte( get_register( RP_HL ) ) );
    cycles = 8;
}

void SM83::MOV_mRP_RG( void ) {
    // Special Case!
    if( opcode == 0xEA ) {
        write_byte( fetch_word(), A );
        cycles = 16;
        return;
    }
    write_byte( get_register( RP_HL ), get_register( GET_SRC ) );
    cycles = 8;
}

void SM83::HALT( void ) {
    halted = true;
    cycles = 4;
}

void SM83::ADD( void ) {
    uint8_t to_op = 0x00;

    if( opcode == 0x86 ) {
        to_op = read_byte( get_register( RP_HL ) );
        cycles = 8;
    }else if( opcode == 0xC6 ) {
        to_op = fetch_byte();
        cycles = 8;
    }else {
        to_op = get_register( GET_SRC );
        cycles = 4;
    }

    A = internal_add( A, to_op, false );
    set_flag( F_N, false );
}

void SM83::ADC( void ) {
    uint8_t to_op = 0x00;

    if( opcode == 0x8E ) {
        to_op = read_byte( get_register( RP_HL ) );
        cycles = 8;
    }else if( opcode == 0xCE ) {
        to_op = fetch_byte();
        cycles = 8;
    }else {
        to_op = get_register( GET_SRC );
        cycles = 4;
    }

    A = internal_add( A, to_op, get_flag( F_C ) );
    set_flag( F_N, false );
}

void SM83::SUB( void ) {
    uint8_t to_op = 0x00;

    if( opcode == 0x96 ) {
        to_op = read_byte( get_register( RP_HL ) );
        cycles = 8;
    }else if( opcode == 0xD6 ) {
        to_op = fetch_byte();
        cycles = 8;
    }else {
        to_op = get_register( GET_SRC );
        cycles = 4;
    }

    A = internal_add( A, ~to_op, true );
    set_flag( F_C, !get_flag( F_C ) );
    set_flag( F_H, !get_flag( F_H ) );
    set_flag( F_N, true );
}

void SM83::SBC( void ) {
    uint8_t to_op = 0x00;

    if( opcode == 0x9E ) {
        to_op = read_byte( get_register( RP_HL ) );
        cycles = 8;
    }else if( opcode == 0xDE ) {
        to_op = fetch_byte();
        cycles = 8;
    }else {
        to_op = get_register( GET_SRC );
        cycles = 4;
    }

    A = internal_add( A, ~to_op, !get_flag( F_C ) );
    set_flag( F_C, !get_flag( F_C ) );
    set_flag( F_H, !get_flag( F_H ) );
    set_flag( F_N, true );
}

void SM83::AND( void ) {
    uint8_t to_op = 0x00;

    if( opcode == 0xA6 ) {
        to_op = read_byte( get_register( RP_HL ) );
        cycles = 8;
    }else if( opcode == 0xE6 ) {
        to_op = fetch_byte();
        cycles = 8;
    }else {
        to_op = get_register( GET_SRC );
        cycles = 4;
    }

    A &= to_op;
    
    set_flag( F_Z, A == 0x00 );
    set_flag( F_N, false );
    set_flag( F_H, true );
    set_flag( F_C, false );
}

void SM83::XOR( void ) {
    uint8_t to_op = 0x00;

    if( opcode == 0xAE ) {
        to_op = read_byte( get_register( RP_HL ) );
        cycles = 8;
    }else if( opcode == 0xEE ) {
        to_op = fetch_byte();
        cycles = 8;
    }else {
        to_op = get_register( GET_SRC );
        cycles = 4;
    }

    A ^= to_op;
    
    set_flag( F_Z, A == 0x00 );
    set_flag( F_N, false );
    set_flag( F_H, false );
    set_flag( F_C, false );
}

void SM83::OR( void ) {
    uint8_t to_op = 0x00;

    if( opcode == 0xB6 ) {
        to_op = read_byte( get_register( RP_HL ) );
        cycles = 8;
    }else if( opcode == 0xF6 ) {
        to_op = fetch_byte();
        cycles = 8;
    }else {
        to_op = get_register( GET_SRC );
        cycles = 4;
    }

    A |= to_op;
    
    set_flag( F_Z, A == 0x00 );
    set_flag( F_N, false );
    set_flag( F_H, false );
    set_flag( F_C, false );
}

void SM83::CP( void ) {
    uint8_t to_op = 0x00;

    if( opcode == 0xBE ) {
        to_op = read_byte( get_register( RP_HL ) );
        cycles = 8;
    }else if( opcode == 0xFE ) {
        to_op = fetch_byte();
        cycles = 8;
    }else {
        to_op = get_register( GET_SRC );
        cycles = 4;
    }

    uint8_t tmp_byte = ( uint8_t )( A - to_op );
    
    set_flag( F_Z, tmp_byte == 0x00 );
    set_flag( F_N, true );
    set_flag( F_H, ( int )( ( A & 0x0F ) - ( tmp_byte & 0x0F ) ) < 0 );
    set_flag( F_C, A < to_op );
}

void SM83::RET( void ) {
    if( parse_conditional( GET_COND ) ) {
        PC = pop_from_stack();
        cycles = 20;
    }else {
        cycles = 8;
    }
}

void SM83::POP( void ) {
    set_register( GET_DST, pop_from_stack() );
    cycles = 12;
}

void SM83::JUMP( void ) {
    // Special case!
    if( opcode == 0xE9 ) {
        PC = get_register( RP_HL );
        cycles = 4;
        return;
    }

    uint16_t addr = fetch_word();
    if( parse_conditional( GET_COND ) ) {
        cycles = 16;
        PC = addr;
    }else {
        cycles = 12;
    }
}

void SM83::CALL( void ) {
    uint16_t addr = fetch_word();
    if( parse_conditional( GET_COND ) ) {
        cycles = 24;
        push_to_stack( PC );
        PC = addr;
    }else {
        cycles = 12;
    }
}

void SM83::PUSH( void ) {
    push_to_stack( get_register( GET_SRC ) );
    cycles = 16;
}

void SM83::RST( void ) {
    push_to_stack( PC );

    cycles = 16;

    switch( opcode ) {
        case 0xC7: PC = 0x0000; break;
        case 0xCF: PC = 0x0008; break;
        case 0xD7: PC = 0x0010; break;
        case 0xDF: PC = 0x0018; break;
        case 0xE7: PC = 0x0020; break;
        case 0xEF: PC = 0x0028; break;
        case 0xF7: PC = 0x0030; break;
        case 0xFF: PC = 0x0038; break;
        default: UNI(); break;
    }
}

void SM83::RETI( void ) {
    interrupt_enabled = true;
    PC = pop_from_stack();
    cycles = 16;
}

void SM83::LDH( void ) {
    uint8_t offset = 0x00;
    switch( opcode ) {
        case 0xE0:
            offset = fetch_byte();
            write_byte( ( 0xFF00 + offset ), A );
            cycles = 12;
            break;
        case 0xE2:
            write_byte( ( 0xFF00 + C ), A );
            cycles = 16;
            break;
        case 0xF0:
            offset = fetch_byte();
            A = read_byte( ( 0xFF00 + offset ) );
            cycles = 12;
            break;
        case 0xF2:
            A = read_byte( ( 0xFF00 + C ) );
            cycles = 16;
            break;
        default: UNI(); break;
    }
}

void SM83::ADD_SP_IMM( void ) {
    int8_t offset = ( int8_t )fetch_byte();
    int res = int( SP + offset );

    set_flag( F_Z, false );
    set_flag( F_N, false );
    set_flag( F_H, ( ( SP ^ offset ^ ( res & 0xFFFF ) ) & 0x10 ) == 0x10 );
    set_flag( F_C, ( ( SP ^ offset ^ ( res & 0xFFFF ) ) & 0x100 ) == 0x100 );

    if( opcode == 0xE8 ) {
        SP = res & 0xFFFF;
        cycles = 16;
    }else if( opcode == 0xF8 ) {
        set_register( RP_HL, res );
        cycles = 12;
    }
}

void SM83::DI( void ) {
    interrupt_enabled = false;
    cycles = 4;
}

void SM83::EI( void ) {
    ei_pending = true;
    cycles = 4;
}

void SM83::LD_SP_HL( void ) {
    SP = get_register( RP_HL );
    cycles = 8;
}

/* CB Opcodes */

void SM83::CB_OPCODES( void ) {
    // Fetch CB Opcode
    opcode = fetch_byte();

    // Execute CB opcode
    ( this->*cb_opcodes[opcode].operate )();
}

void SM83::RLC( void ) { 
    uint8_t to_op = 0x00;
    if( opcode == 0x06 ) {
        cycles = 16;
        to_op = read_byte( get_register( RP_HL ) );
    }else {
        cycles = 8;
        to_op = get_register( CB_GET_DST );
    }
    
    set_flag( F_C, to_op & 0x80 );
    to_op <<= 1;

    if( get_flag( F_C ) ) {
        to_op |= 0x01;
    }

    set_flag( F_Z, to_op == 0x00 );
    set_flag( F_N, false );
    set_flag( F_H, false );

    if( opcode == 0x06 ) {
        write_byte( get_register( RP_HL ), to_op );
    }else {
        set_register( CB_GET_DST, to_op );
    }
}

void SM83::RRC( void ) {
    uint8_t to_op = 0x00;
    if( opcode == 0x0E ) {
        cycles = 16;
        to_op = read_byte( get_register( RP_HL ) );
    }else {
        cycles = 8;
        to_op = get_register( CB_GET_DST );
    }
    
    set_flag( F_C, to_op & 0x01 );
    to_op >>= 1;

    if( get_flag( F_C ) ) {
        to_op |= 0x80;
    }

    set_flag( F_Z, to_op == 0x00 );
    set_flag( F_N, false );
    set_flag( F_H, false );

    if( opcode == 0x0E ) {
        write_byte( get_register( RP_HL ), to_op );
    }else {
        set_register( CB_GET_DST, to_op );
    }
}

void SM83::RL( void ) { 
    bool old_cy = get_flag( F_C );

    uint8_t to_op = 0x00;
    if( opcode == 0x16 ) {
        cycles = 16;
        to_op = read_byte( get_register( RP_HL ) );
    }else {
        cycles = 8;
        to_op = get_register( CB_GET_DST );
    }
    
    set_flag( F_C, to_op & 0x80 );
    to_op <<= 1;

    if( old_cy ) {
        to_op |= 0x01;
    }

    set_flag( F_Z, to_op == 0x00 );
    set_flag( F_N, false );
    set_flag( F_H, false );

    if( opcode == 0x16 ) {
        write_byte( get_register( RP_HL ), to_op );
    }else {
        set_register( CB_GET_DST, to_op );
    }
 }

void SM83::RR( void ) { 
    bool old_cy = get_flag( F_C );

    uint8_t to_op = 0x00;
    if( opcode == 0x1E ) {
        cycles = 16;
        to_op = read_byte( get_register( RP_HL ) );
    }else {
        cycles = 8;
        to_op = get_register( CB_GET_DST );
    }
    
    set_flag( F_C, to_op & 0x01 );
    to_op >>= 1;

    if( old_cy ) {
        to_op |= 0x80;
    }

    set_flag( F_Z, to_op == 0x00 );
    set_flag( F_N, false );
    set_flag( F_H, false );

    if( opcode == 0x1E ) {
        write_byte( get_register( RP_HL ), to_op );
    }else {
        set_register( CB_GET_DST, to_op );
    }
 }

void SM83::SLA( void ) { 
    uint8_t to_op = 0x00;
    if( opcode == 0x26 ) {
        cycles = 16;
        to_op = read_byte( get_register( RP_HL ) );
    }else {
        cycles = 8;
        to_op = get_register( CB_GET_DST );
    }
    
    set_flag( F_C, to_op & 0x80 );
    to_op <<= 1;

    set_flag( F_Z, to_op == 0x00 );
    set_flag( F_N, false );
    set_flag( F_H, false );

    if( opcode == 0x26 ) {
        write_byte( get_register( RP_HL ), to_op );
    }else {
        set_register( CB_GET_DST, to_op );
    }
}

void SM83::SRA( void ) { 
    uint8_t to_op = 0x00;
    if( opcode == 0x2E ) {
        cycles = 16;
        to_op = read_byte( get_register( RP_HL ) );
    }else {
        cycles = 8;
        to_op = get_register( CB_GET_DST );
    }
    
    bool old_msb = to_op & 0x80;
    set_flag( F_C, to_op & 0x01 );
    to_op >>= 1;
    
    // MSB remains unchanged
    if( old_msb ) {
        to_op |= 0x80;
    }

    set_flag( F_Z, to_op == 0x00 );
    set_flag( F_N, false );
    set_flag( F_H, false );

    if( opcode == 0x2E ) {
        write_byte( get_register( RP_HL ), to_op );
    }else {
        set_register( CB_GET_DST, to_op );
    }
}

void SM83::SWAP( void ) {
    uint8_t to_op = 0x00;
    if( opcode == 0x36 ) {
        cycles = 16;
        to_op = read_byte( get_register( RP_HL ) );
    }else {
        cycles = 8;
        to_op = get_register( CB_GET_DST );
    }
    
    uint8_t tmp_byte = to_op >> 4;
    to_op = ( to_op << 4 ) | tmp_byte;

    set_flag( F_Z, to_op == 0x00 );
    set_flag( F_N, false );
    set_flag( F_H, false );
    set_flag( F_C, false );

    if( opcode == 0x36 ) {
        write_byte( get_register( RP_HL ), to_op );
    }else {
        set_register( CB_GET_DST, to_op );
    }
}

void SM83::SRL( void ) {
    uint8_t to_op = 0x00;
    if( opcode == 0x3E ) {
        cycles = 16;
        to_op = read_byte( get_register( RP_HL ) );
    }else {
        cycles = 8;
        to_op = get_register( CB_GET_DST );
    }
    
    set_flag( F_C, to_op & 0x01 );
    to_op >>= 1;

    set_flag( F_Z, to_op == 0x00 );
    set_flag( F_N, false );
    set_flag( F_H, false );

    if( opcode == 0x3E ) {
        write_byte( get_register( RP_HL ), to_op );
    }else {
        set_register( CB_GET_DST, to_op );
    }
}

void SM83::BIT( void ) { 
    uint8_t to_op = 0x00;
    if( ( ( opcode & 0x0F ) == 0x06 ) || ( ( opcode & 0x0F ) == 0x0E ) ) {
        cycles = 12;
        to_op = read_byte( get_register( RP_HL ) );
    }else {
        cycles = 8;
        to_op = get_register( CB_GET_DST );
    }

    set_flag( F_Z, !( to_op & ( 1 << CB_GET_OPT ) ) );
    set_flag( F_N, false );
    set_flag( F_H, true );
}

void SM83::RES( void ) {
    if( ( ( opcode & 0x0F ) == 0x06 ) || ( ( opcode & 0x0F ) == 0x0E ) ) {
        cycles = 12;
        write_byte( get_register( RP_HL ), read_byte( get_register( RP_HL ) ) & ~( 1 << CB_GET_OPT ) );
    }else {
        cycles = 8;
        set_register( CB_GET_DST, get_register( CB_GET_DST ) & ~( 1 << CB_GET_OPT ) );
    }
}

void SM83::SET( void ) {
    if( ( ( opcode & 0x0F ) == 0x06 ) || ( ( opcode & 0x0F ) == 0x0E ) ) {
        cycles = 12;
        write_byte( get_register( RP_HL ), read_byte( get_register( RP_HL ) ) | ( 1 << CB_GET_OPT ) );
    }else {
        cycles = 8;
        set_register( CB_GET_DST, get_register( CB_GET_DST ) | ( 1 << CB_GET_OPT ) );
    }
}
