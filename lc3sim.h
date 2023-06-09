#ifndef  LC3_SIMULATOR_H
#define  LC3_SIMULATOR_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define LC3_REGISTER_COUNT        8
#define LC3_MEMORY_ADDRESSABILITY 2
#define LC3_MEMORY_SIZE           65536
#define LC3_ENTRY_POINT           0
#define ZERO_FLAG 2
#define NEGATIVE_FLAG 4
#define POSITIVE_FLAG 1
#define SIGN_BITMASK 0x8000
#define BIT4_MASK 0x0010
#define ONE_EXTEND 0xFFE0
#define BIT8_MASK 0x0100
#define FAIL 0
#define SUCCESS 1
#define PRINT_REGISTERS 0  // 1 to print registers debugging info




// State.
typedef struct {
  int16_t register_array[LC3_REGISTER_COUNT];
  uint16_t pc;
  uint8_t nzp;
  uint16_t * memory;
} lc3_machine_state;

typedef union {
  uint16_t flat;
  struct opcode_only_type {
    uint16_t unused : 12;
    uint16_t opcode : 4;
  } opcode_only;
  struct add_and_reg_type {
    uint16_t sr2 : 3;
    uint16_t unused : 2;
    uint16_t selection_bit : 1;
    uint16_t sr1 : 3;
    uint16_t dr : 3;
    uint16_t opcode : 4;
  } add_and_reg;
  struct add_and_imm_type {
    uint16_t immediate : 5;
    uint16_t selection_bit : 1;
    uint16_t sr1 : 3;
    uint16_t dr : 3;
    uint16_t opcode : 4;
  } add_and_imm;
  struct br_type {
    uint16_t pc_offset : 9;
    uint16_t nzp : 3;  // gave a weird bug when i set the type as uint8_t
    uint16_t opcode : 4;
  } br;
  struct jmp_type{
    uint16_t unused6 : 6;
    uint16_t base_r  : 3;
    uint16_t unused3 : 3;
    uint16_t opcode  : 4;
  } jmp;
  struct jsr_imm_type{
    uint16_t pc_offset : 11;
    uint16_t selection_bit : 1;
    uint16_t opcode : 4; 
  } jsr_imm;
  struct jsr_reg_type {
    uint16_t unused6 : 6;
    uint16_t base_r  : 3;
    uint16_t unused2 : 2;
    uint16_t selection_bit : 1;
    uint16_t opcode  : 4;
  } jsr_reg;
  struct ld_ldi_type {
    uint16_t pc_offset : 9;
    uint16_t dr : 3;
    uint16_t opcode : 4;
  } ld_ldi;
  struct ldr_type {
    uint16_t offset : 6;
    uint16_t base_r : 3;
    uint16_t dr : 3;
    uint16_t opcode : 4;
  } ldr;
  struct lea_type {
    uint16_t pc_offset : 9;
    uint16_t dr : 3;
    uint16_t opcode : 4;
  } lea;
  struct not_ret_type {
    uint16_t unused : 6;
    uint16_t sr : 3;
    uint16_t dr : 3;
    uint16_t opcode : 4;
  } not_ret;
  struct st_sti_type {
    uint16_t pc_offset : 9;
    uint16_t sr : 3;
    uint16_t opcode : 4;
  } st_sti;
  struct str_type {
    uint16_t offset : 6;
    uint16_t base_r : 3;
    uint16_t sr : 3;
    uint16_t opcode : 4;
  } str;
} lc3_instruction;

// Instructions
#define ADD_OPCODE 0b0001
#define AND_OPCODE 0b0101
#define BR_OPCODE 0b0000
#define JMP_OPCODE 0b1100
#define JSR_OPCODE 0b0100
#define LD_OPCODE 0b0010
#define LDI_OPCODE 0b1010
#define LDR_OPCODE 0b0110
#define LEA_OPCODE 0b1110
#define NOT_OPCODE 0b1001
#define ST_OPCODE 0b0011
#define STI_OPCODE 0b1011
#define STR_OPCODE 0b0111

// does it normally sext with 0s?
// Forward declarations
void lc3_init(lc3_machine_state * lc3);
void run_next_instruction(lc3_machine_state * lc3);
void set_flags(lc3_machine_state * lc3, int16_t reg_val);
int16_t sext(int16_t offset, uint16_t sign_bit);

void add_register(lc3_machine_state * lc3, lc3_instruction instruction);
void add_immediate(lc3_machine_state * lc3, lc3_instruction instruction);
void and_register(lc3_machine_state * lc3, lc3_instruction instruction);
void and_immediate(lc3_machine_state * lc3, lc3_instruction instruction);
void br(lc3_machine_state * lc3, lc3_instruction instruction);
void jmp(lc3_machine_state * lc3, lc3_instruction instruction);
void jsr_imm(lc3_machine_state * lc3, lc3_instruction instruction);
void jsr_reg(lc3_machine_state * lc3, lc3_instruction instruction);
void ld(lc3_machine_state * lc3, lc3_instruction instruction);
void ldi(lc3_machine_state * lc3, lc3_instruction instruction);
void ldr(lc3_machine_state * lc3, lc3_instruction instruction);
void lea(lc3_machine_state * lc3, lc3_instruction instruction);
void not(lc3_machine_state * lc3, lc3_instruction instruction);
void ret(lc3_machine_state * lc3, lc3_instruction instruction);
void st(lc3_machine_state * lc3, lc3_instruction instruction);
void sti(lc3_machine_state * lc3, lc3_instruction instruction);
void str(lc3_machine_state * lc3, lc3_instruction instruction);
void invalid_opcode(void);

//tests
void run_tests(lc3_machine_state * lc3);
int add_test(lc3_machine_state *lc3);
int add_test1(lc3_machine_state * lc3);
int add_test2(lc3_machine_state * lc3);
int and_test(lc3_machine_state * lc3);
int BRn_test(lc3_machine_state * lc3);
int jmp_test(lc3_machine_state * lc3);
int jsr_test(lc3_machine_state * lc3);
int jsrr_test(lc3_machine_state * lc3);
int ld_test(lc3_machine_state * lc3);
int ldi_test(lc3_machine_state * lc3);
int ldr_test(lc3_machine_state * lc3);
int lea_test(lc3_machine_state * lc3);
int not_test(lc3_machine_state * lc3);
int ret_test(lc3_machine_state * lc3);
int st_test(lc3_machine_state * lc3);
int sti_test(lc3_machine_state * lc3);
int str_test(lc3_machine_state * lc3);





void run_print_instructions(lc3_machine_state * state, size_t num);
void print_register_state(lc3_machine_state * state);
void reset_pc(lc3_machine_state * lc3);
#endif

