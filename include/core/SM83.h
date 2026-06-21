#pragma once

// Extract Information from INSTRUCTION data
#define GET_DST opcodes[opcode].dst
#define GET_SRC opcodes[opcode].src
#define GET_COND opcodes[opcode].cond
#define GET_OPT opcodes[opcode].option
#define CB_GET_DST cb_opcodes[opcode].dst
#define CB_GET_SRC cb_opcodes[opcode].src
#define CB_GET_COND cb_opcodes[opcode].cond
#define CB_GET_OPT cb_opcodes[opcode].option

#include <cstdint>
#include <vector>

// Declare registers structure
struct Registers {
  union {
    struct {
      uint8_t c;
      uint8_t b;
    };
    uint16_t bc;
  };

  union {
    struct {
      uint8_t e;
      uint8_t d;
    };
    uint16_t de;
  };

  union {
    struct {
      uint8_t l;
      uint8_t h;
    };
    uint16_t hl;
  };

  union {
    struct {
      uint8_t f;
      uint8_t a;
    };
    uint16_t af;
  };

  uint16_t pc;
  uint16_t sp;
};

// Forward declare Bus class
class IBus;

class SM83 {
 public:
  /* Constructors */
  SM83(void);
  ~SM83(void);

  void DBG(void);

  /* Emulation */
  int step(void);
  void reset(void);

  /* Emulation */
  bool halted;
  bool error;
  bool tmp_interrupt;
  uint8_t opcode;
  unsigned int cycles;

  /* Memory */
  IBus* bus = nullptr;
  void connect_to_bus(IBus* b) { bus = b; }
  uint8_t read_byte(uint16_t addr);
  uint16_t read_word(uint16_t addr);
  uint8_t fetch_byte(void);
  uint16_t fetch_word(void);
  void write_byte(uint16_t addr, uint8_t data);
  void write_word(uint16_t addr, uint16_t data);

  /* Flags */
  enum FLAG {
    F_Z = (1 << 7),  // Zero Flag
    F_N = (1 << 6),  // Negative Flag
    F_H = (1 << 5),  // Half-Carry Flag
    F_C = (1 << 4)   // Carry Flag
  };
  void set_flag(FLAG f, bool val);
  bool get_flag(FLAG f);
  bool will_carry(uint8_t bit_no, uint16_t a, uint16_t b, bool cy);

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
  Registers regs;

  // Define Conditionals
  enum COND {
    CN_NONE,  // No condition
    CN_Z,     // If Zero
    CN_NZ,    // If Not Zero
    CN_H,     // If Half
    CN_NH,    // If Not Half
    CN_N,     // If Negative
    CN_NN,    // If Not Negative
    CN_C,     // If Carry
    CN_NC     // If Not Carry
  };
  bool parse_conditional(COND cond);

  /* Utility */
  uint8_t internal_add(uint8_t a, uint8_t b, bool cy);
  uint16_t pop_from_stack(void);
  void push_to_stack(uint16_t data);

  /* Opcodes */
  void UNI(void);

  void NOP(void);
  void LD_RP_IMM(void);
  void LD_mRP_A(void);
  void INC_RP(void);
  void INC_RG(void);
  void DEC_RG(void);
  void LD_RG_IMM(void);
  void RLCA(void);
  void LD_mRP_SP(void);
  void ADD_HL_RP(void);
  void LD_A_mRP(void);
  void DEC_RP(void);
  void RRCA(void);
  void STOP(void);
  void RLA(void);
  void JUMP_REL(void);
  void RRA(void);
  void DAA(void);
  void CPL(void);
  void SCF(void);
  void CCF(void);
  void MOV_RG_RG(void);
  void MOV_RG_mRP(void);
  void MOV_mRP_RG(void);
  void HALT(void);
  void ADD(void);
  void ADC(void);
  void SUB(void);
  void SBC(void);
  void AND(void);
  void XOR(void);
  void OR(void);
  void CP(void);
  void RET(void);
  void POP(void);
  void JUMP(void);
  void CALL(void);
  void PUSH(void);
  void RST(void);
  void RETI(void);
  void LDH(void);
  void ADD_SP_IMM(void);
  void DI(void);
  void EI(void);
  void LD_SP_HL(void);

  /* CB Opcodes */
  void CB_OPCODES(void);
  void RLC(void);
  void RRC(void);
  void RL(void);
  void RR(void);
  void SLA(void);
  void SRA(void);
  void SWAP(void);
  void SRL(void);
  void BIT(void);
  void RES(void);
  void SET(void);

  /* Instructions */
  struct INSTRUCTION {
    // Pointer to instruction's opcode function
    void (SM83::*operate)(void) = nullptr;
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
};