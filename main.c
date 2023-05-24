#include <stdio.h>
#include <stdint.h>

#include "lc3sim.h"

void print_register_state(lc3_machine_state * state) {
    for (size_t i = 0; i < LC3_REGISTER_COUNT; i++) {
        printf("Register %d: 0x%x\n", i, state->register_array[i]);
    }
}

int main(int argc, char** argv) {
    // Init machine.
    //
    lc3_machine_state lc3;

    lc3_init(&lc3);

    // Preload the memory with some interesting instructions.
    //
    lc3.memory[0] = 0b0101000000100000;
    lc3.memory[1] = 0b0001000000100001;
    lc3.memory[2] = 0b0101001001100000;
    lc3.memory[3] = 0b0001001001100010;
    lc3.memory[4] = 0b0001010000000001;

    // Run five instructions.
    //
    for (size_t i = 0; i < 5; i++) {
        run_next_instruction(&lc3);
        print_register_state(&lc3)
    }

    return 0;
}