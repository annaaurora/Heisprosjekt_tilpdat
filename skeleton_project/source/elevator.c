/**
 * @file elevator.c
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
				hardware_command_floor_indicator_on(j);
				return true;
			}
		}
		return false;
}





