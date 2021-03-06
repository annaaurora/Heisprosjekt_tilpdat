#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "elevator.h"

typedef enum {
	init_state,
	waiting_state,
	move_up_state,
	move_down_state,
	at_floor_state,
	stop_programme,
} State;

State current_state;

void controller_state_machine();

void controller_decide_up_or_down_state();

void controller_moving_up();

void controller_moving_down();

void controller_at_floor();

void controller_stop_button();

#endif
