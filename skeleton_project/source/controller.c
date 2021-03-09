#include "elevator.h"
#include "controller.h"

/*for å starte heisen. Trenger oppdatere ordre da det skjer så fort*/
void controller_initialize(void){

	printf("stopper den her");
	int error = hardware_init();
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

	elevator_clear_all_order_lights();

	for(int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; i++){
		up_orders[i] = 0;
		cab_orders[i] = 0;
		down_orders[i] = 0;
	}

	printf("HEI");

	hardware_command_movement(HARDWARE_MOVEMENT_UP);
	direction = up;
	
	printf("HALLo");

	bool at_floor = false;
	while(!at_floor){
		at_floor = elevator_check_if_at_floor();
	}

	hardware_command_movement(HARDWARE_MOVEMENT_STOP);

	printf("halla");

	current_state = waiting_state;

	printf("kom deg hit!!");

}

/*init funker som den skal*/


/*for bestemme heisen retning*/
void controller_decide_up_or_down(){

	while(!elevator_orders_exist()){
		if(hardware_read_stop_signal()){
			current_state = stop_programme;
			break;
		}
	}
	

	if(elevator_orders_exist()){
		int next_floor = elevator_floor_with_order();
		printf("%d", next_floor);
		

		if((next_floor - current_floor) > 0){
			current_state = move_up_state;
		}

		else if((next_floor - current_floor) < 0){
			current_state = move_down_state;
		}

		else {
			current_state = at_floor_state;
		}
	}
}

/*sette retning oppover. Velge ordre, videre til at_floor_state*/
void controller_moving_up(){

	hardware_command_movement(HARDWARE_MOVEMENT_UP);
	direction = up;

	/*current floor har riktig verdi*/
	
	bool up_or_cab_orders_above = false;
	for(int j = current_floor; j < HARDWARE_NUMBER_OF_FLOORS; j++){
		if(up_orders[j] == 1 || cab_orders[j] == 1){
			up_or_cab_orders_above = true;
		}
	}

	printf("%d", up_or_cab_orders_above);
	
	while(elevator_orders_exist()){

		if(hardware_read_stop_signal()){
			current_state = stop_programme;
			break;
		}

		if(up_or_cab_orders_above){
			for(int i = current_floor; i < HARDWARE_NUMBER_OF_FLOORS; i++){
				if(hardware_read_floor_sensor(i)){
					hardware_command_floor_indicator_on(i);
					if((up_orders[i] == 1) || (cab_orders[i] == 1)){
						current_floor = i;
						current_state = at_floor_state;
						break;
					}
				}
			}
			break;
		}
		else{
			for(int n = current_floor; n < HARDWARE_NUMBER_OF_FLOORS; n++){
				if(hardware_read_floor_sensor(n)){
					hardware_command_floor_indicator_on(n);
					if((down_orders[n] == 1)){
						current_floor = n;
						current_state = at_floor_state;
						break;
					}
				}
			}
			break;
		}
		
	}
}

/*sette retning nedover. Velge ordre, videre til at_floor_state*/
void controller_moving_down(){

	hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
	direction = down;

	printf("current floor: %d", current_floor);	

	/*riktig for cab orders, men ikke for down...*/

	bool down_or_cab_orders_below = false;
	for(int j = 0; j <= current_floor; j++){
		if(down_orders[j] == 1 || cab_orders[j] == 1){
			down_or_cab_orders_below = true;
		}
	}

	printf("%d", down_or_cab_orders_below);

	while(elevator_orders_exist()){

		if(hardware_read_stop_signal()){
			current_state = stop_programme;
			break;
		}

		if(down_or_cab_orders_below){
			for(int i = current_floor; i >= 0; i--){
				if(hardware_read_floor_sensor(i)){
					hardware_command_floor_indicator_on(i);
					if((down_orders[i] == 1) || (cab_orders[i] == 1)){
						current_floor = i;
						current_state = at_floor_state;
						break;
					}
				}
			}
			break;
		}
		else{
			for(int n = current_floor; n >= 0; n--){
				if(hardware_read_floor_sensor(n)){
					hardware_command_floor_indicator_on(n);
					if((up_orders[n] == 1)){
						current_floor = n;
						current_state = at_floor_state;
						break;
					}
				}
			}
			break;
		}
	}
}


void controller_at_floor(){

	/*nullstille ordre*/
	up_orders[current_floor] = 0;
	hardware_command_order_light(current_floor, HARDWARE_ORDER_UP, 0);
	cab_orders[current_floor] = 0;
	hardware_command_order_light(current_floor, HARDWARE_ORDER_INSIDE, 0);
	down_orders[current_floor] = 0;
	hardware_command_order_light(current_floor, HARDWARE_ORDER_DOWN, 0);

	hardware_command_movement(HARDWARE_MOVEMENT_STOP);

	hardware_command_door_open(1);

	int start_time = (clock() * 1000)/CLOCKS_PER_SEC;

	int end_time = start_time + 3000;

	bool stop_elevator = false;

	do {
		start_time = (clock() * 1000)/CLOCKS_PER_SEC;

		elevator_update_orders();

		if(hardware_read_obstruction_signal()){
			end_time = start_time + 3000;
		}
		if(hardware_read_stop_signal()){
			current_state = stop_programme;
			stop_elevator = true;
			break; 
		}
	} while(start_time <= end_time);

	hardware_command_door_open(0);

	while(elevator_orders_exist() && stop_elevator == false){
		
		bool orders_above = false;
		for(int j = current_floor; j < HARDWARE_NUMBER_OF_FLOORS; j++){
			if(up_orders[j] == 1 || cab_orders[j] == 1 || down_orders[j] == 1){
				orders_above = true;
			}
		}
		
		/*sjekke om ordre over eller under den retningen heisen går i*/
		if(direction == up && orders_above == true){
			current_state = move_up_state;
			break;
		}

		else if(direction == down && orders_above == false){
			current_state = move_down_state;
			break;
		}
		
		/*endre retning*/
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
	
	if(!elevator_orders_exist() && stop_elevator == false){
		current_state = waiting_state;
	}
}


void controller_stop_button(){
	hardware_command_movement(HARDWARE_MOVEMENT_STOP);
	while(hardware_read_stop_signal()){	
		hardware_command_stop_light(1);
	}
	hardware_command_stop_light(0);

	elevator_clear_all_order_lights();

	for(int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; i++){
		up_orders[i] = 0;
		cab_orders[i] = 0;
		down_orders[i] = 0;
	}
	current_state = waiting_state;
}



void controller_state_machine(){
	while(true){

		switch(current_state){

			case init_state :
				printf("entering init_state\n");
				controller_initialize();
				break;
			

			case waiting_state :
				printf("entering waiting_state\n");
				controller_decide_up_or_down();
				break;
			

			case move_up_state :
				printf("entering move_up_state\n");
				controller_moving_up();
				break;
			

			case move_down_state :
				controller_moving_down();
				break;
			

			case at_floor_state :
				printf("entering at_floor_state\n");
				controller_at_floor();
				break;

			case stop_programme :
				printf("entering stop_programme_state\n");
				controller_stop_button();
				break;

			
			default:
				hardware_command_movement(HARDWARE_MOVEMENT_STOP);
		
			}
	}
}








