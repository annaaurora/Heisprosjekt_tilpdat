/**
 * @file queue.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-03-17
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include "elevator.h"
#include "hardware.h"
#include "time.h"

/*clear alle ordre*/
void elevator_clear_all_orders(void){
	for(int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; i++){
		up_orders[i] = 0;
		cab_orders[i] = 0;
		down_orders[i] = 0;
	}
}


/*clear orders bare i current floor*/
void elevator_clear_orders_at_current_floor(void){
	up_orders[current_floor] = 0;
	hardware_command_order_light(current_floor, HARDWARE_ORDER_UP, 0);
	cab_orders[current_floor] = 0;
	hardware_command_order_light(current_floor, HARDWARE_ORDER_INSIDE, 0);
	down_orders[current_floor] = 0;
	hardware_command_order_light(current_floor, HARDWARE_ORDER_DOWN, 0);
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


bool elevator_orders_in_order_type(int order_type[]){
	elevator_update_orders();
	for(int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; i++){
		if(order_type[i] == 1){
			return true;
		}
	}
	return false;
}


bool elevator_orders_exist(void){
	return (elevator_orders_in_order_type(up_orders) || elevator_orders_in_order_type(cab_orders) || elevator_orders_in_order_type(down_orders));
}


/*sjekke hvilken etasje som har ordre (brukes bare i waiting, når det kun er max 1 ordre)*/
int elevator_floor_with_order(){
	for(int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; i++){
		if((up_orders[i] == 1) || (cab_orders[i] == 1) || (down_orders[i] == 1)){
			return i;
		}
	}
	return 0;
}


int elevator_find_top_floor_with_down_order(void){
	int top_floor_with_order;
	for(int i = current_floor; i < HARDWARE_NUMBER_OF_FLOORS; i++){
		if(down_orders[i] == 1){
			top_floor_with_order = i;
		}
	}
	return top_floor_with_order;
}


int elevator_find_bottom_floor_with_up_order(void){
	int bottom_floor_with_order;
	for(int i = current_floor; i >= 0; i--){
		if(up_orders[i] == 1){
			bottom_floor_with_order = i;
		}
	}
	return bottom_floor_with_order;
}