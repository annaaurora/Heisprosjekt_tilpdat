#include "elevator.h"
#include "controller.h"

/*for å starte heisen. Trenger oppdatere ordre da det skjer så fort*/
void controller_initialize(void){

	int error = hardware_init();
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

	elevator_clear_all_order_lights();

	for(int i = 0; i < 4; i++){
		up_orders[i] = 0;
		cab_orders[i] = 0;
		down_orders[i] = 0;
	}

	hardware_command_movement(HARDWARE_MOVEMENT_UP);
	direction = up;
	
	bool at_floor = false;
	while(!at_floor){
		at_floor = elevator_check_if_at_floor();
	}

	hardware_command_movement(HARDWARE_MOVEMENT_STOP);

	current_state = waiting_state;

}


void controller_decide_up_or_down(){

	while(!elevator_orders_exist()){
		if(hardware_read_stop_signal()){
			current_state = stop_programme;
			break;
		}
	}

	if(elevator_orders_exist()){
		int next_floor = elevator_floor_with_order();

		if((next_floor - current_floor) > 0){
			current_state = move_up_state;
		}

		else if((next_floor - current_floor) < 0){
			current_state = move_down_state;
		}

		else{
			current_state = at_floor_state;
		}
	}
}


void controller_moving_up(){
	
	hardware_command_movement(HARDWARE_MOVEMENT_UP);
	direction = up;
	
	while(elevator_orders_exist()){

		if(hardware_read_stop_signal()){
			current_state = stop_programme;
			break;
		}

		for(int i = current_floor; i < HARDWARE_NUMBER_OF_FLOORS; i++){
			if(hardware_read_floor_sensor(i)){
				hardware_command_floor_indicator_on(i);
				if((up_orders[i] = 1) | (cab_orders[i] = 1)){
					current_floor = i;
					current_state = at_floor_state;
					break;
				}
			}
		}
	}
}


void controller_moving_down(){

	hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
	direction = down;

	while(elevator_orders_exist()){

		if(hardware_read_stop_signal()){
			current_state = stop_programme;
			break;
		}

		for(int i = 0; i < current_floor; i++){
			if(hardware_read_floor_sensor(i)){
				hardware_command_floor_indicator_on(i);
				if((down_orders[i] = 1) | (cab_orders[i] = 1)){
					current_floor = i;
					current_state = at_floor_state;
					break;
				}
			}
		}
	}
}

/*At_floor:
	- Stoppe heis
	- Åpne dør
	- 3 sek
	- Lukke dør
	- Slette ordre
	- Hvis ordre:
		○ Direction = up -> sjekk om cab orders eller up-orders over
		○ Motsatt hvis direction down
		○ Hvis ingen -> ta nærmeste ordre
Hvis to like nærme -> ta den i samme retning*/


void controller_at_floor(){

	up_orders[current_floor] = 0;
	cab_orders[current_floor] = 0;
	down_orders[current_floor] = 0;

	while(elevator_orders_exist()){

		hardware_command_movement(HARDWARE_MOVEMENT_STOP);

		hardware_command_door_open(1);

		int start_time = (clock() * 1000)/CLOCKS_PER_SEC;
	
		int end_time = start_time + 3000;

		do {
			start_time = (clock() * 1000)/CLOCKS_PER_SEC;
			if(hardware_read_obstruction_signal()){
				end_time = start_time + 3000;
			}
			if(hardware_read_stop_signal()){
				current_state = stop_programme;
				break;
			}
		} while(start_time <= end_time);

		hardware_command_door_open(0);
		
		if(hardware_read_stop_signal()){
			current_state = stop_programme;
			break;
		}

		if(direction == up){
			for(int i = current_floor ; i < HARDWARE_NUMBER_OF_FLOORS; i++){
				if((up_orders[i] = 1 )| (cab_orders[i] = 1)){
					current_state = move_up_state;
					break;
				}
			}
			break;
		}

		else if(direction == down){
			for(int j = 0; j < current_floor; j++){
				if((down_orders[j] = 1) | (cab_orders[j] = 1)){
					current_state = move_down_state;
					break;
				}
			}
			break;
		}

		else{
			int next_floor = elevator_closest_floor_with_order();
			if((current_floor - next_floor) > 0){
				current_state = move_up_state;
				break;
			}
			else if((current_floor - next_floor) < 0){
				current_state = move_down_state;
				break;
			}
		}
	}
	
	if(!elevator_orders_exist()){
		current_state = waiting_state;
	}
}


void controller_stop_button(){
	hardware_command_movement(HARDWARE_MOVEMENT_STOP);
	while(hardware_read_stop_signal()){	
		hardware_command_stop_light(1);
	}
	current_state = init_state;
}



void controller_state_machine(){
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

			case stop_programme :
				controller_stop_button();
				break;

			
			default:
				hardware_command_movement(HARDWARE_MOVEMENT_STOP);
		
			}
	}
}








