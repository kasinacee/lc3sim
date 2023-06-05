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
  int16_t immediate = sext(instruction.add_and_imm.immediate, 4);
  
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

    int16_t pc_offset = sext(instruction.br.pc_offset,8);

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

  int16_t pc_offset = sext(instruction.jsr_imm.pc_offset, 10);

  lc3->pc += pc_offset;

}

void jsr_reg(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc += 1;
  lc3->register_array[7] = lc3->pc;

  lc3->pc = lc3->register_array[instruction.jsr_reg.base_r];
}

void ld(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc +=1;

  int16_t pc_offset = sext(instruction.ld_ldi.pc_offset, 8);

  lc3->register_array[instruction.ld_ldi.dr] = lc3->memory[lc3->pc + pc_offset];

  uint16_t dr = instruction.ld_ldi.dr;
  set_flags(lc3, lc3->register_array[dr]);

}

void ldi(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc +=1;

  int16_t pc_offset = sext(instruction.ld_ldi.pc_offset, 8);

  uint16_t memory_index = lc3->memory[lc3->pc + pc_offset];
  lc3->register_array[instruction.ld_ldi.dr] = lc3->memory[memory_index];

  uint16_t dr = instruction.ld_ldi.dr;
  set_flags(lc3, lc3->register_array[dr]);

}

void ldr(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc += 1;

  uint16_t dr = instruction.ldr.dr;
  uint16_t base_r = instruction.ldr.base_r;
  int16_t offset = sext(instruction.ldr.offset,5);
  
  lc3->register_array[dr] = lc3->memory[lc3->register_array[base_r] + offset];

  set_flags(lc3, lc3->register_array[dr]);

}

void lea(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc +=1;

  uint16_t dr = instruction.lea.dr;
  int16_t pc_offset = sext(instruction.lea.pc_offset, 8);
  
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
  int16_t pc_offset = sext(instruction.st_sti.pc_offset, 8);
  lc3->memory[lc3->pc + pc_offset] = lc3->register_array[sr];
}

void sti(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc += 1;
  uint16_t sr = instruction.st_sti.sr;
  int16_t pc_offset = sext(instruction.st_sti.pc_offset, 8);
  uint16_t memory_index = lc3->memory[lc3->pc + pc_offset];
  lc3->memory[memory_index] = lc3->register_array[sr];
}

void str(lc3_machine_state * lc3, lc3_instruction instruction){
  lc3->pc += 1;
  int16_t offset = sext(instruction.str.offset, 5);
  uint16_t base_r = instruction.str.base_r;
  uint16_t sr = instruction.str.sr;

  lc3->memory[lc3->register_array[base_r] + offset] = lc3->register_array[sr];

}

void set_flags(lc3_machine_state * lc3, int16_t reg_val){
   if(reg_val == 0){
     lc3->nzp = ZERO_FLAG;
   }else if((reg_val & SIGN_BITMASK) == SIGN_BITMASK) {
     lc3->nzp = NEGATIVE_FLAG;
   } else{
     lc3->nzp = POSITIVE_FLAG;
   }

}

int16_t sext(int16_t offset, uint16_t sign_bit){
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

/*
R0 = 1, R1 = 2, R2 = R1 + R0. checks that R2 = 3 and nzp = 001
*/
int add_test(lc3_machine_state * lc3){
  int result = FAIL;
  // Preload the memory with some interesting instructions.
  lc3->memory[0] = 0b0101000000100000; 
  lc3->memory[1] = 0b0001000000100001; //R0 = 1
  lc3->memory[2] = 0b0101001001100000; 
  lc3->memory[3] = 0b0001001001100010; //R1 = 2
  lc3->memory[4] = 0b0001010000000001; //R2 = R1 + R0

  
  run_print_instructions(lc3, 5);

  if(lc3->register_array[2] == 3 && lc3->nzp == POSITIVE_FLAG){
    result = SUCCESS;
  }
  
  printf("add_test: %d \n", result);

  reset_pc(lc3);

  return result;
} 

/*
R0 = -16, R1 = 15, R2 = R1 + R0, checks that R2 = -1 and nzp = 100
*/
int add_test1(lc3_machine_state * lc3){
  int result = FAIL;
  lc3->memory[0] = 0b0101000000100000; 
  lc3->memory[1] = 0b0001000000110000; //R0 = -16
  lc3->memory[2] = 0b0101001001100000; 
  lc3->memory[3] = 0b0001001001101111; //R1 = 15
  lc3->memory[4] = 0b0001010000000001; //R2 = R1 + R0

  run_print_instructions(lc3, 5);
  if(lc3->register_array[2] == -1 && lc3->nzp == NEGATIVE_FLAG){
    result = SUCCESS;
  }
  printf("add1_test: %d \n", result);
  reset_pc(lc3);
  return result;

}

int add_test2(lc3_machine_state * lc3){
  int result = FAIL;
  // Preload the memory with some interesting instructions.
  lc3->memory[0] = 0b0101000000100000; 
  lc3->memory[1] = 0b0001000000100010; //R0 = 2
  lc3->memory[2] = 0b0001000000111110; //R0 = R0 -2

  
  run_print_instructions(lc3, 3);

  if(lc3->register_array[0] == 0 && lc3->nzp == ZERO_FLAG){
    result = SUCCESS;
  }
  
  printf("add_test2: %d \n", result);

  reset_pc(lc3);

  return result;
} 

/*
R0 = 0xFF00, R1 = 0x00FF, R2 = R1 & R0. 
Checks that R2 = 0, nzp = 010
*/
int and_test(lc3_machine_state * lc3){
  int result = FAIL;
  
  lc3->register_array[0] = 0xFF00;
  lc3->register_array[1] = 0x00FF;

  lc3->memory[0] = 0b0101010000000001; //R2 = R1 & R0

  run_print_instructions(lc3, 1);

  if(lc3->register_array[2] == 0 && lc3->nzp == ZERO_FLAG){
    result = SUCCESS;
  }
  
  printf("AND_test: %d \n", result);

  reset_pc(lc3);

  return result;
} 

/*
BRn test
*/
int BRn_test(lc3_machine_state * lc3){
  int result = FAIL;

  lc3->memory[0] = 0b0101000000100000; 
  lc3->memory[1] = 0b0001000000110000; //R0 = -16
  lc3->memory[2] = 0b0101001001100000; 
  lc3->memory[3] = 0b0001001001101111; //R1 = 15
  lc3->memory[4] = 0b0001010000000001; //R2 = R1 + R0 = -1
  lc3->memory[5] = 0b0000100000001000; //BRn (pc = pc + 8), final pc value should be 14
 
  run_print_instructions(lc3, 6);

  if(lc3->pc == 14){
    result = SUCCESS;
  }

  printf("BRn_test: %d \n", result);

  reset_pc(lc3);

  return result;

}

int jmp_test(lc3_machine_state * lc3){
  int result = FAIL;
  // Preload the memory with some interesting instructions.
  lc3->memory[0] = 0b0101101101100000; 
  lc3->memory[1] = 0b0001101101101000; //R5 = 8
  lc3->memory[2] = 0b1100000101000000; //PC <- R5

  
  run_print_instructions(lc3, 3);

  if(lc3->pc == 8){
    result = SUCCESS;
  }
  
  printf("jmp_test: %d \n", result);

  reset_pc(lc3);

  return result;
} 

int jsr_test(lc3_machine_state * lc3){
  int result = FAIL;
  // Preload the memory with some interesting instructions.
  lc3->memory[0] = 0b0101101101100000; 
  lc3->memory[1] = 0b0001101101100100; //R5 = 4
  lc3->memory[2] = 0b0100100000001001; //PC<- PC+9, pc=3+9

  
  run_print_instructions(lc3, 3);

  if(lc3->pc == 12  && lc3->register_array[7] == 3){
    result = SUCCESS;
  }
  
  printf("jsr_test: %d \n", result);

  reset_pc(lc3);

  return result;
} 

int jsrr_test(lc3_machine_state * lc3){
  int result = FAIL;
  // Preload the memory with some interesting instructions.
  lc3->memory[0] = 0b0101101101100000; 
  lc3->memory[1] = 0b0001101101100101; //R5 = 5
  lc3->memory[2] = 0b0100000101000000; //PC<- R5

  
  run_print_instructions(lc3, 3);

  if(lc3->pc == 5  && lc3->register_array[7] == 3){
    result = SUCCESS;
  }
  
  printf("jsrr_test: %d \n", result);

  reset_pc(lc3);

  return result;
} 

int ld_test(lc3_machine_state * lc3){
  int result = FAIL;
  // Preload the memory with some interesting instructions.
  lc3->memory[10] = 0xFF00; 
  lc3->memory[0] = 0b0010110000001001; //R6<- M[PC+9], pc=1

  
  run_print_instructions(lc3, 1);

  if(lc3->nzp == NEGATIVE_FLAG  && lc3->register_array[6] == (int16_t)0xFF00){
    result = SUCCESS;
  }
  
  printf("ld_test: %d \n", result);

  reset_pc(lc3);

  return result;
} 

int ldi_test(lc3_machine_state * lc3){
  int result = FAIL;
  // Preload the memory with some interesting instructions.
  lc3->memory[10] = 0xF0F0; 
  lc3->memory[0] = 0b1010110000001001; //R6<- M[M[PC+9]], pc=1
  lc3->memory[0xF0F0] = 0x0ABC;

  
  run_print_instructions(lc3, 1);

  if(lc3->nzp == POSITIVE_FLAG  && lc3->register_array[6] == (int16_t)0x0ABC){
    result = SUCCESS;
  }
  
  printf("ldi_test: %d \n", result);

  reset_pc(lc3);

  return result;
} 

int ldr_test(lc3_machine_state * lc3){
  int result = FAIL;
  // Preload the memory with some interesting instructions.
  lc3->memory[10] = 0xF00F; 
  lc3->memory[0] = 0b0101101101100000; 
  lc3->memory[1] = 0b0001101101101111; //R5 = 15
  lc3->memory[2] = 0b0110110101111011; //R6<- M[R5-5], pc=1
  
  
  run_print_instructions(lc3, 3);

  if(lc3->nzp == NEGATIVE_FLAG  && lc3->register_array[6] == (int16_t)0xF00F){
    result = SUCCESS;
  }
  
  printf("ldr_test: %d \n", result);

  reset_pc(lc3);

  return result;
} 

int lea_test(lc3_machine_state * lc3){
  int result = FAIL;
  // Preload the memory with some interesting instructions. 
  lc3->memory[0] = 0b0101101101100000; 
  lc3->memory[1] = 0b0001101101101110; //R5 = 14
  lc3->memory[2] = 0b1110110000000111; //R6<- PC+7, pc=3
  
  
  run_print_instructions(lc3, 3);

  if(lc3->nzp == POSITIVE_FLAG  && lc3->register_array[6] == (int16_t)10){
    result = SUCCESS;
  }
  
  printf("lea_test: %d \n", result);

  reset_pc(lc3);

  return result;
} 

int not_test(lc3_machine_state * lc3){
  int result = FAIL;
  // Preload the memory with some interesting instructions. 
  lc3->register_array[0] = 0x0FF0;
  lc3->memory[0] = 0b1001000000111111;
  
  
  run_print_instructions(lc3, 1);

  if(lc3->nzp == NEGATIVE_FLAG  && lc3->register_array[0] == (int16_t)0xF00F){
    result = SUCCESS;
  }
  
  printf("not_test: %d \n", result);

  reset_pc(lc3);

  return result;
} 

int ret_test(lc3_machine_state * lc3){
  int result = FAIL;
  // Preload the memory with some interesting instructions. 
  lc3->memory[0] = 0b0101111111100000; 
  lc3->memory[1] = 0b0001111111101100; //R7 = 12
  lc3->memory[2] = 0b1100000111000000; //PC <- R7
  
  
  run_print_instructions(lc3, 3);

  if(lc3->pc == 12){
    result = SUCCESS;
  }
  
  printf("ret_test: %d \n", result);

  reset_pc(lc3);

  return result;
} 

int st_test(lc3_machine_state * lc3){
  int result = FAIL;
  // Preload the memory with some interesting instructions. 
  lc3->register_array[2] = 0xCAFE;
  lc3->memory[0] = 0b0011010111111111; // M[pc-1] <-R2
  
  
  run_print_instructions(lc3, 1);

  if(lc3->memory[0] == (uint16_t)0xCAFE){
    result = SUCCESS;
  }
  
  printf("st_test: %d \n", result);

  reset_pc(lc3);

  return result;
} 

int sti_test(lc3_machine_state * lc3){
  int result = FAIL;
  // Preload the memory with some interesting instructions. 
  lc3->register_array[2] = 0xBEEF;
  lc3->memory[10] = 0x5;
  lc3->memory[0] = 0b1011010000001001; // M[M[pc+9]] <-R2, pc=1
  
  
  run_print_instructions(lc3, 1);

  if(lc3->memory[5] == (uint16_t)0xBEEF){
    result = SUCCESS;
  }
  
  printf("sti_test: %d \n", result);

  reset_pc(lc3);

  return result;
} 

int str_test(lc3_machine_state * lc3){
  int result = FAIL;
  // Preload the memory with some interesting instructions. 
  lc3->register_array[2] = 0xBABA;
  lc3->memory[0] = 0b0101101101100000; 
  lc3->memory[1] = 0b0001101101101001; //R5 = 9
  lc3->memory[2] = 0b0111010101111110; // M[R5-2] <-R2
  
  
  run_print_instructions(lc3, 3);

  if(lc3->memory[7] == (uint16_t)0xBABA){
    result = SUCCESS;
  }
  
  printf("str_test: %d \n", result);

  reset_pc(lc3);

  return result;
} 

void run_print_instructions(lc3_machine_state * lc3, size_t num){
  // Run five instructions.
  for (size_t i = 0; i < num; i++) {
    run_next_instruction(lc3);
    if(PRINT_REGISTERS == 1){
      print_register_state(lc3);
    }
  }
}

void print_register_state(lc3_machine_state * state) {
  printf("----------------\n");
  for (size_t i = 0; i < LC3_REGISTER_COUNT; i++) {
      printf("Register %ld: 0x%x | %d \n", i, state->register_array[i], state->register_array[i]);
  }
    
}

void reset_pc(lc3_machine_state * lc3){
  lc3->pc = 0;

}

void run_tests(lc3_machine_state * lc3){
  add_test(lc3);
  add_test1(lc3);
  add_test2(lc3);
  and_test(lc3);
  BRn_test(lc3);
  jmp_test(lc3);
  jsr_test(lc3);
  jsrr_test(lc3);
  ld_test(lc3);
  ldi_test(lc3);
  ldr_test(lc3);
  lea_test(lc3);
  not_test(lc3);
  ret_test(lc3);
  st_test(lc3);
  sti_test(lc3);
  str_test(lc3);
  
}