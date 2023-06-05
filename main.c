#include <stdio.h>
#include <stdint.h>

#include "lc3sim.h"



int main(int argc, char** argv) {
    // Init machine.
    //
    lc3_machine_state lc3;

    lc3_init(&lc3);

    run_tests(&lc3);

    return 0;
}
