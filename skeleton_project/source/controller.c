#include "elevator.h"
#include "controller.h"

/*Initialiserer hardwre og heis. Trenger ikke oppdatere ordre da det skjer så fort*/
void controller_initialize(void){
	/*initialiserer hardware*/
	int error = hardware_init();
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

	/*nullstille ordre-lys*/
	elevator_clear_all_order_lights();

	/*nullstille ordre-arrays*/
	elevator_clear_all_orders();

	/*setter retning oppover og stopper i første etasje den møter*/
	hardware_command_movement(HARDWARE_MOVEMENT_UP);
	direction = up;

	bool at_floor = false;
	while(!at_floor){
		at_floor = elevator_check_if_at_floor(); /*oppdaterer current_floor*/
	}

	/*between_floor usann etter while over*/
	between_floors = false;

	hardware_command_movement(HARDWARE_MOVEMENT_STOP);

	current_state = waiting_state;


}



/*Bestemme heisens retning*/
void controller_decide_up_or_down(void){
	
	/*ingen ordre -> sjekke stopp-knapp, vente på ordre*/
	while(!elevator_orders_exist()){
		if(hardware_read_stop_signal()){
			current_state = stop_programme;
			break;
		}
	}
	
	/*når en ordre kalt -> sjekke om ordren er over eller under for å bestemme retning*/
	/*elevator_orders_exist oppdaterer ordre og returnerer true dersom det finnes ordre*/
	if(elevator_orders_exist()){
		int next_floor = elevator_floor_with_order();

		if((next_floor - current_floor) > 0){
			current_state = move_up_state;
		}

		else if((next_floor - current_floor) < 0){
			current_state = move_down_state;
		}

		/*hvis ordren er i current_floor, må det sjekkses om heisen kan være mellom to etasjer*/
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

/*Retning = oppover. Velge ordre, videre til at_floor_state*/
void controller_moving_up(void){

	/*setter retning opp og oppdaterer direction*/
	hardware_command_movement(HARDWARE_MOVEMENT_UP);
	direction = up;

	/*up_or_cab_orders har førsteprioritet, sjekker derfor om disse finnes over heisen*/
	bool up_or_cab_orders_above = false;
	for(int j = current_floor; j < HARDWARE_NUMBER_OF_FLOORS; j++){
		if(up_orders[j] == 1 || cab_orders[j] == 1){
			up_or_cab_orders_above = true;
		}
	}
	
	/*elevator_orders_exist oppdaterer ordre og returnerer true dersom det finnes ordre*/
	while(elevator_orders_exist()){

		/*sjekker stop-knapp*/
		if(hardware_read_stop_signal()){
			current_state = stop_programme;
			break;
		}

		/*hvis up_or_can_orders_above = true*/
		/*at_floor_state når heisen kommer til en etg med ordre*/
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

		/*hvis up_or_can_orders_above = false*/
		/*at_floor_state når heisen kommer til en etg med ordre*/
		else{
			for(int n = current_floor; n < HARDWARE_NUMBER_OF_FLOORS; n++){
				if(hardware_read_floor_sensor(n)){
					hardware_command_floor_indicator_on(n);
					current_floor = n;
					if((down_orders[n] == 1)){
						current_state = at_floor_state;
						break;
					}
				}
			}
			break;
		}
		
	}
}

/*Retning = nedover. Velge ordre, videre til at_floor_state*/
void controller_moving_down(void){

	/*Sette retning ned opp oppdatere direction*/
	hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
	direction = down;

	/*down_or_cab_orders har førsteprioritet, sjekker derfor om disse finnes under heisen*/
	bool down_or_cab_orders_below = false;
	for(int j = 0; j <= current_floor; j++){
		if(down_orders[j] == 1 || cab_orders[j] == 1){
			down_or_cab_orders_below = true;
		}
	}

	 /*elevator_orders_exist oppdaterer ordre og returnerer true dersom det finnes ordre*/
	while(elevator_orders_exist()){

		/*sjekker stopp-knapp*/
		if(hardware_read_stop_signal()){
			current_state = stop_programme;
			break;
		}

		/*hvis down_or_can_orders_above = true*/
		/*at_floor_state når heisen kommer til en etg med ordre*/
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

		/*hvis down_or_can_orders_above = false*/
		/*at_floor_state når heisen kommer til en etg med ordre*/
		else{
			for(int n = current_floor; n >= 0; n--){
				if(hardware_read_floor_sensor(n)){
					hardware_command_floor_indicator_on(n);
					current_floor = n;
					if((up_orders[n] == 1)){
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

	/*nullstille alle ordre og ordre-lys på current_floor*/
	elevator_clear_orders_at_current_floor();

	/*stoppe heisen*/
	hardware_command_movement(HARDWARE_MOVEMENT_STOP);

	/*åpne døren*/
	hardware_command_door_open(1);

	/*timer : definere start- og sluttid*/
	int start_time = (clock() * 1000)/CLOCKS_PER_SEC;

	int end_time = start_time + 3000;

	bool stop_elevator = false;

	/*mens timeren teller ned oppdateres ordrene, og obstruction- og stoppknapp sjekkes*/
	/*obstruction nullstiller timeren*/
	do {
		start_time = (clock() * 1000)/CLOCKS_PER_SEC;

		elevator_update_orders();

		elevator_clear_orders_at_current_floor();

		if(hardware_read_obstruction_signal()){
			end_time = start_time + 3000;
		}
		if(hardware_read_stop_signal()){
			current_state = stop_programme;
			stop_elevator = true;
			break; 
		}
	} while(start_time <= end_time);

	/*lukke døren*/
	hardware_command_door_open(0);

	/*bestemme heisens retning*/
	while(elevator_orders_exist() && stop_elevator == false){
		
		/*sjekke om det er ordre over heisen.*/
		bool orders_above = false;
		for(int j = current_floor; j < HARDWARE_NUMBER_OF_FLOORS; j++){
			if(up_orders[j] == 1 || cab_orders[j] == 1 || down_orders[j] == 1){
				orders_above = true;
			}
		}
		/*hvis orders_above = false, finnes det ordre under heisen*/
		
		/*hvis det finnes ordre i heisen retning skal heisen fortsette i samme retning*/
		if(direction == up && orders_above == true){
			current_state = move_up_state;
			break;
		}

		else if(direction == down && orders_above == false){
			current_state = move_down_state;
			break;
		}
		
		/*hvis det ikke finnes ordre i heisen retning må heisen endre retning*/
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
	
	/*hvis det ikke finnes ordre skal heisen i waiting_state*/
	if(!elevator_orders_exist() && stop_elevator == false){
		current_state = waiting_state;
	}
}

/*stoppe heisen når stopp-knappen trykkes inn*/
void controller_stop_button(void){
	/*stoppe heisen*/
	hardware_command_movement(HARDWARE_MOVEMENT_STOP);

	/*så lenge stopp-knappen er trykket inn skal stopp-signalet lyset og døren være åpen*/
	while(hardware_read_stop_signal()){	
		hardware_command_stop_light(1); 
		if(elevator_check_if_at_floor()){
			hardware_command_door_open(1);
		}
	}

	/*knapp sluppet -> stopp-lys av*/
	hardware_command_stop_light(0);

	/*dør åpen i 3 sekunder*/
	int start_time = (clock() * 1000)/CLOCKS_PER_SEC;
	int end_time = start_time + 3000;

	if(elevator_check_if_at_floor()){
		do{
			start_time = (clock() * 1000)/CLOCKS_PER_SEC;
			if(hardware_read_obstruction_signal()){
				end_time = start_time + 3000;
		} while(start_time <= end_time);
	}

	else{
		between_floors = true;
	}

	hardware_command_door_open(0);

	elevator_clear_all_order_lights();

	elevator_clear_all_orders();

	current_state = waiting_state;
}



void controller_state_machine(void){
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
				/*printf("entering move_up_state\n");*/
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








