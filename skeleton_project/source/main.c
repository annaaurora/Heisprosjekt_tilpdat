#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"
#include "elevator.h"
#include "controller.h"



int main(){

    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");

	printf("her kommer beskjeder\n");

	current_state = init_state;

	printf("current state er satt til init\n");

	controller_state_machine();

	printf("state machine er ferdigkjørt\n");

    return 0;

}
