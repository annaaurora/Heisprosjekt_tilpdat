/**
 * @file controller.c
 * @brief Implementation file for state machine and state machine functions.
 */

#include "elevator.h"
#include "queue.h"
#include "controller.h"


void controller_initialize(void){
	
	int error = hardware_init();
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

	elevator_clear_all_order_lights();

	queue_clear_all_orders();

	hardware_command_movement(HARDWARE_MOVEMENT_UP);
	direction = up;

	bool at_floor = false;
	while(!at_floor){
		at_floor = elevator_check_if_at_floor();
	}

	between_floors = false;

	hardware_command_movement(HARDWARE_MOVEMENT_STOP);

	current_state = waiting_state;


}


void controller_decide_up_or_down(void){
	
	while(!queue_orders_exist()){
		if(hardware_read_stop_signal()){
			current_state = stop_state;
			break;
		}
	}
	
	if(queue_orders_exist()){
		int next_floor = queue_floor_with_order();

		if((next_floor - current_floor) > 0){
			current_state = move_up_state;
		}

		else if((next_floor - current_floor) < 0){
			current_state = move_down_state;
		}

		else {
			if(between_floors == false){
				current_state = at_floor_state;
			}
			else{
				if(direction == up){
					current_state = move_down_state;
				}
				else{
					current_state = move_up_state;
				}
			}
		}
	}
}


void controller_moving_up(void){

	bool any_orders_above = false;
	for(int j = current_floor; j < HARDWARE_NUMBER_OF_FLOORS; j++){
		if(up_orders[j] == 1 || cab_orders[j] == 1 || down_orders[j] == 1){
			any_orders_above = true;
		}
	}

	hardware_command_movement(HARDWARE_MOVEMENT_UP);
	direction = up;

	bool up_or_cab_orders_above = false;
	for(int j = current_floor; j < HARDWARE_NUMBER_OF_FLOORS; j++){
		if(up_orders[j] == 1 || cab_orders[j] == 1){
			up_or_cab_orders_above = true;
		}
	}
	
	while(queue_orders_exist()){
		if(any_orders_above == false){
			current_state = waiting_state;
			break;
		}

		if(hardware_read_stop_signal()){
			current_state = stop_state;
			break;
		}

		if(up_or_cab_orders_above){
			for(int i = current_floor; i < HARDWARE_NUMBER_OF_FLOORS; i++){
				if(hardware_read_floor_sensor(i)){
					hardware_command_floor_indicator_on(i);
					current_floor = i;
					if((up_orders[i] == 1) || (cab_orders[i] == 1)){
						current_state = at_floor_state;
						break;
					}
				}
			}
			break;
		}


		else{
			int top_floor_with_order = queue_find_top_floor_with_down_order();
				for(int i = current_floor; i < HARDWARE_NUMBER_OF_FLOORS; i++){
					if(hardware_read_floor_sensor(i)){
						hardware_command_floor_indicator_on(i);
						current_floor = i;
						if(hardware_read_floor_sensor(top_floor_with_order)){
							hardware_command_floor_indicator_on(top_floor_with_order);
							current_floor = top_floor_with_order;
							current_state = at_floor_state;
							break;
						}
					}
				}
			break;
		}
	}
}


void controller_moving_down(void){

	bool any_orders_below = false;
	for(int j = 0; j <= current_floor; j++){
		if(up_orders[j] == 1 || cab_orders[j] == 1 || down_orders[j] == 1){
			any_orders_below = true;
		}
	}

	hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
	direction = down;

	bool down_or_cab_orders_below = false;
	for(int j = 0; j <= current_floor; j++){
		if(down_orders[j] == 1 || cab_orders[j] == 1){
			down_or_cab_orders_below = true;
		}
	}

	while(queue_orders_exist()){
		if(any_orders_below == false){
			current_state = waiting_state;
			break;
		}

		if(hardware_read_stop_signal()){
			current_state = stop_state;
			break;
		}

		if(down_or_cab_orders_below){
			for(int i = current_floor; i >= 0; i--){
				if(hardware_read_floor_sensor(i)){
					hardware_command_floor_indicator_on(i);
					current_floor = i;
					if((down_orders[i] == 1) || (cab_orders[i] == 1)){
						current_state = at_floor_state;
						break;
					}
				}
			}
			break;
		}

		else{
			int bottom_floor_with_order = queue_find_bottom_floor_with_up_order();
				for(int i = current_floor; i >= 0; i--){
					if(hardware_read_floor_sensor(i)){
						hardware_command_floor_indicator_on(i);
						current_floor = i;
						if(hardware_read_floor_sensor(bottom_floor_with_order)){
							hardware_command_floor_indicator_on(bottom_floor_with_order);
							current_floor = bottom_floor_with_order;
							current_state = at_floor_state;
							break;
						}
					}
				}
			break;
		}
	}
}


void controller_at_floor(void){

	queue_clear_orders_at_current_floor();

	hardware_command_movement(HARDWARE_MOVEMENT_STOP);

	hardware_command_door_open(1);

	int start_time = (clock() * 1000)/CLOCKS_PER_SEC;
	int end_time = start_time + 3000;

	bool stop_elevator = false;

	do {
		start_time = (clock() * 1000)/CLOCKS_PER_SEC;

		queue_update_orders();

		queue_clear_orders_at_current_floor();

		if(hardware_read_obstruction_signal()){
			end_time = start_time + 3000;
		}
		if(hardware_read_stop_signal()){
			current_state = stop_state;
			stop_elevator = true;
			break; 
		}
	} while(start_time <= end_time);

	hardware_command_door_open(0);

	while(queue_orders_exist() && stop_elevator == false){
		
		bool orders_above = false;
		for(int j = current_floor; j < HARDWARE_NUMBER_OF_FLOORS; j++){
			if(up_orders[j] == 1 || cab_orders[j] == 1 || down_orders[j] == 1){
				orders_above = true;
			}
		}
		
		if(direction == up && orders_above == true){
			current_state = move_up_state;
			break;
		}

		else if(direction == down && orders_above == false){
			current_state = move_down_state;
			break;
		}
		
		else{
			if(direction == up){
				current_state = move_down_state;
				break;
			}
			if(direction == down){
				current_state = move_up_state;
				break;
			}
		}
	}
	
	if(!queue_orders_exist() && stop_elevator == false){
		current_state = waiting_state;
	}
}


void controller_stop_button(void){
	
	queue_clear_all_orders();
	elevator_clear_all_order_lights();
	hardware_command_movement(HARDWARE_MOVEMENT_STOP);

	while(hardware_read_stop_signal()){	
		hardware_command_stop_light(1); 
		if(elevator_check_if_at_floor()){
			hardware_command_door_open(1);
		}
	}

	hardware_command_stop_light(0);

	int start_time = (clock() * 1000)/CLOCKS_PER_SEC;
	int end_time = start_time + 3000;

	if(elevator_check_if_at_floor()){
		do{
			start_time = (clock() * 1000)/CLOCKS_PER_SEC;
			if(hardware_read_obstruction_signal()){
				end_time = start_time + 3000;
			}
		} while(start_time <= end_time);
	}

	else{
		between_floors = true;
	}

	hardware_command_door_open(0);

	current_state = waiting_state;
}


void controller_state_machine(void){
	while(true){

		switch(current_state){

			case init_state :
				controller_initialize();
				break;
			

			case waiting_state :
				controller_decide_up_or_down();
				break;
			

			case move_up_state :
				controller_moving_up();
				break;
			

			case move_down_state :
				controller_moving_down();
				break;
			

			case at_floor_state :
				controller_at_floor();
				break;

			case stop_state :
				controller_stop_button();
				break;

			
			default:
				hardware_command_movement(HARDWARE_MOVEMENT_STOP);
		
			}
	}
}








