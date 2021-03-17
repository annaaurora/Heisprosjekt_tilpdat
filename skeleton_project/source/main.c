#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"
#include "elevator.h"
#include "controller.h"

/**
 * @brief 
 * 
 * @return int 
 */



int main(){

	current_state = init_state;

	controller_state_machine();

    return 0;

}
