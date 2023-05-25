#include "lc3sim.h"
//small test change

void lc3_init(lc3_machine_state * lc3) {
	lc3->memory = (uint16_t *)malloc(LC3_MEMORY_ADDRESSABILITY*LC3_MEMORY_SIZE);
	lc3->pc     = LC3_ENTRY_POINT;
  
}

void run_next_instruction(lc3_machine_state * lc3) {
  uint16_t pc = lc3->pc;
  lc3_instruction next_instruction;
  next_instruction.flat = lc3->memory[pc];
  
  uint16_t opcode = next_instruction.opcode_only.opcode;
  switch(opcode) {

    case  ADD_OPCODE:
      if(next_instruction.add_and_reg.selection_bit == 1){
         //immediate add
        add_immediate(lc3, next_instruction);
      }else{
        //register add
        add_register(lc3, next_instruction);
      }
      break; 

    case AND_OPCODE:
      if(next_instruction.add_and_reg.selection_bit == 1){
        and_immediate(lc3, next_instruction);
      }else{
        and_register(lc3, next_instruction);
      }
      break;

    case BR_OPCODE:
      br(lc3, next_instruction);
      break;

    case JMP_OPCODE:
      jmp(lc3, next_instruction);
      break;

    case JSR_OPCODE:
      if(next_instruction.jsr_imm.selection_bit == 1){
        jsr_imm(lc3, next_instruction);
      }else{
        jsr_reg(lc3, next_instruction);
      }
      break;
    
    case LD_OPCODE:
      ld(lc3, next_instruction);
      break;
    
    case LDI_OPCODE:
      ldi(lc3, next_instruction);
      break;

    case LDR_OPCODE:
      ldr(lc3, next_instruction);
      break;
    
    case LEA_OPCODE:
      lea(lc3, next_instruction);
      break;
    
    case NOT_OPCODE:
      not(lc3, next_instruction);
      break;
    
    case ST_OPCODE:
      st(lc3, next_instruction);
      break;
    
    case STI_OPCODE:
      sti(lc3, next_instruction);
      break;
    
    case STR_OPCODE:
      str(lc3, next_instruction);
      break;
    
   /* you can have any number of case statements */
   default :
   		invalid_opcode();
	 }
}

void add_register(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc += 1;
  uint16_t dr = instruction.add_and_reg.dr;
  uint16_t sr1 = instruction.add_and_reg.sr1;
  uint16_t sr2 = instruction.add_and_reg.sr2;
  
  lc3->register_array[dr] = lc3->register_array[sr1] + lc3->register_array[sr2];
  
  set_flags(lc3, lc3->register_array[dr]);
}

void add_immediate(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc += 1;
  uint16_t dr = instruction.add_and_imm.dr;
  uint16_t sr1 = instruction.add_and_imm.sr1;
  uint16_t immediate = sext(instruction.add_and_imm.immediate, 4);

  lc3->register_array[dr] = lc3->register_array[sr1] + immediate;
  
  set_flags(lc3, lc3->register_array[dr]);

}

void and_register(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc += 1;
  uint16_t dr = instruction.add_and_reg.dr;
  uint16_t sr1 = instruction.add_and_reg.sr1;
  uint16_t sr2 = instruction.add_and_reg.sr2;
  
  lc3->register_array[dr] = lc3->register_array[sr1] & lc3->register_array[sr2];
  
  set_flags(lc3, lc3->register_array[dr]);
}

void and_immediate(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc += 1;
  uint16_t dr = instruction.add_and_imm.dr;
  uint16_t sr1 = instruction.add_and_imm.sr1;
  uint16_t immediate = sext(instruction.add_and_imm.immediate, 4);
  
  lc3->register_array[dr] = lc3->register_array[sr1] & immediate;
  set_flags(lc3, lc3->register_array[dr]);

}

void br(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc += 1; //i should be incrementing this here right?
  if((lc3->nzp & instruction.br.nzp) != 0){

    uint16_t pc_offset = sext(instruction.br.pc_offset,8);

    lc3->pc += pc_offset;
  }
  
}

void jmp(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc += 1;
  lc3->pc = lc3->register_array[instruction.jmp.base_r];
}

void jsr_imm(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc += 1;
  lc3->register_array[7] = lc3->pc;

  uint16_t pc_offset = sext(instruction.jsr_imm.pc_offset, 10);

  lc3->pc += pc_offset;

}

void jsr_reg(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc += 1;
  lc3->register_array[7] = lc3->pc;

  lc3->pc = lc3->register_array[instruction.jsr_reg.base_r];
}

void ld(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc +=1;

  uint16_t pc_offset = sext(instruction.ld_ldi.pc_offset, 8);

  lc3->register_array[instruction.ld_ldi.dr] = lc3->memory[lc3->pc + pc_offset];

  uint16_t dr = instruction.ld_ldi.dr;
  set_flags(lc3, lc3->register_array[dr]);

}

void ldi(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc +=1;

  uint16_t pc_offset = sext(instruction.ld_ldi.pc_offset, 8);

  uint16_t memory_index = lc3->memory[lc3->pc + pc_offset];
  lc3->register_array[instruction.ld_ldi.dr] = lc3->memory[memory_index];

  uint16_t dr = instruction.ld_ldi.dr;
  set_flags(lc3, lc3->register_array[dr]);

}

void ldr(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc += 1;

  uint16_t dr = instruction.ldr.dr;
  uint16_t base_r = instruction.ldr.base_r;
  uint16_t offset = sext(instruction.ldr.offset,5);
  
  lc3->register_array[dr] = lc3->memory[lc3->register_array[base_r] + offset];

  set_flags(lc3, lc3->register_array[dr]);

}

void lea(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc +=1;

  uint16_t dr = instruction.lea.dr;
  uint16_t pc_offset = sext(instruction.lea.pc_offset, 8);
  
  lc3->register_array[dr] = lc3->pc + pc_offset;
  //why do they do it this way? if they already know address
  // of memory label why cant they just return that

}

void not(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc +=1;

  uint16_t dr = instruction.not_ret.dr;
  uint16_t sr = instruction.not_ret.sr;

  lc3->register_array[dr] = ~(lc3->register_array[sr]);
  set_flags(lc3, lc3->register_array[dr]);
}

void ret(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc += 1;
  lc3->pc = lc3->register_array[7];
}

void st(lc3_machine_state* lc3, lc3_instruction instruction){
  lc3->pc += 1;
  uint16_t sr = instruction.st_sti.sr;
  uint16_t pc_offset = sext(instruction.st_sti.pc_offset, 8);
  lc3->memory[lc3->pc + pc_offset] = lc3->register_array[sr];
}

void sti(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc += 1;
  uint16_t sr = instruction.st_sti.sr;
  uint16_t pc_offset = sext(instruction.st_sti.pc_offset, 8);
  uint16_t memory_index = lc3->memory[lc3->pc + pc_offset];
  lc3->memory[memory_index] = lc3->register_array[sr];
}

void str(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc += 1;
  uint16_t offset = sext(instruction.str.offset, 5);
  uint16_t base_r = instruction.str.base_r;
  uint16_t sr = instruction.str.sr;

  lc3->memory[lc3->register_array[base_r] + offset] = lc3->register_array[sr];

}

void set_flags(lc3_machine_state * lc3, uint16_t reg_val){
   if(reg_val == 0){
     lc3->nzp = ZERO_FLAG;
   }else if((reg_val & SIGN_BITMASK) == SIGN_BITMASK) {
     lc3->nzp = NEGATIVE_FLAG;
   } else{
     lc3->nzp = POSITIVE_FLAG;
   }

}

uint16_t sext(uint16_t offset, uint16_t sign_bit){
  uint16_t signbit_bitmask = 0x01 << sign_bit;

  uint16_t one_extend_bitmask = ~((0x01 << sign_bit)-1);

  if((offset & signbit_bitmask) == signbit_bitmask){
    offset = offset | one_extend_bitmask;
  }

  return offset;
}

void invalid_opcode() {
    printf("Invalid opcode.\n");
}

void add_test(){
  
}