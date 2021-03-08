#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include "elevator.h"
#include "hardware.h"
#include "time.h"


/*clear alle ordre-lys*/
void elevator_clear_all_order_lights(void){
    HardwareOrder order_types[3] = {
        HARDWARE_ORDER_UP,
        HARDWARE_ORDER_INSIDE,
        HARDWARE_ORDER_DOWN
    };

    for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
        for(int i = 0; i < 3; i++){
            HardwareOrder type = order_types[i];
            hardware_command_order_light(f, type, 0);
        }
    }
}


/*sjekke om heisen er i en etasje*/
bool elevator_check_if_at_floor(void){
		for(int j = 0; j < HARDWARE_NUMBER_OF_FLOORS; j++){
			if(hardware_read_floor_sensor(j)){
				current_floor = j;
				return true;
			}
		}
		return false;
}




/*oppdaterer de globale variablene up-, cab-, og down_orders*/
/*skrur på ordre-lys*/
void elevator_update_orders(void){ 
	for(int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; i++){
		for(int HardwareOrder = HARDWARE_ORDER_UP; HardwareOrder <= HARDWARE_ORDER_DOWN; ++HardwareOrder){
			if(hardware_read_order(i, HardwareOrder)){
				if(HardwareOrder == HARDWARE_ORDER_UP){
					up_orders[i] = 1;
					hardware_command_order_light(i, HARDWARE_ORDER_UP, 1);
				}
				else if(HardwareOrder == HARDWARE_ORDER_INSIDE){
					cab_orders[i] = 1;
					hardware_command_order_light(i, HARDWARE_ORDER_INSIDE, 1);
				}
				else if(HardwareOrder == HARDWARE_ORDER_DOWN){
					down_orders[i] = 1;
					hardware_command_order_light(i, HARDWARE_ORDER_DOWN, 1);
				}
			}
		}
	}
}




bool elevator_check_for_orders(int order_type[]){
	elevator_update_orders();
	for(int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; i++){
		if(order_type[i] == 1){
			return true;
		}
	}
	return false;
}



bool elevator_orders_exist(void){
	return (elevator_check_for_orders(up_orders) || elevator_check_for_orders(cab_orders) || elevator_check_for_orders(down_orders));
}



int elevator_floor_with_order(){
	for(int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; i++){
		if((up_orders[i] == 1) || (cab_orders[i] == 1) || (down_orders[i] == 1)){
			return i;
		}
	}
	return 0;
}



/*
int elevator_closest_floor_with_order(){
	int closest_order_above;
	for(int i = current_floor + 1; i < HARDWARE_NUMBER_OF_FLOORS; i++){
		if(down_orders[i] == 1){
			closest_order_above = i;
		}
	}

	int closest_order_below;
	for(int i = (current_floor - 1); i > 0; i--){
		if(up_orders[i] == 1){
			closest_order_below = i;
		}
	}

	int closest_order;

	if((closest_order_above - current_floor) < (current_floor - closest_order_below)){
		closest_order = closest_order_above;
	}

	else if((closest_order_above - current_floor) > (current_floor - closest_order_below)){
		closest_order = closest_order_below;
	}

	else {
		if(direction == up){
			closest_order = closest_order_above;
		}
		else{
			closest_order = closest_order_below;
		}
	}

	return closest_order;
}
*/








