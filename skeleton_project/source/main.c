#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"
#include "elevator.h"
#include "controller.h"



int main(){

    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");

	current_state = init_state;

	controller_state_machine();

    return 0;

}
