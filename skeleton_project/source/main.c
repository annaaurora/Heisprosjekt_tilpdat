/**
 * @file main.c
 * @brief main file, sets current state to init state to initialize elevator at the beginning.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"
#include "elevator.h"
#include "controller.h"
#include "queue.h"


int main(){

	current_state = init_state;

	controller_state_machine();

    return 0;

}
