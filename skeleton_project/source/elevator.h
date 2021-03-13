#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include "hardware.h"
#include "time.h"

int current_floor;

typedef enum {
	up,
	down
} elevator_direction;


elevator_direction direction; 


int up_orders[HARDWARE_NUMBER_OF_FLOORS];
int cab_orders[HARDWARE_NUMBER_OF_FLOORS];
int down_orders[HARDWARE_NUMBER_OF_FLOORS];


void elevator_clear_all_order_lights(void);

void elevator_clear_all_orders(void);

void elevator_clear_orders_at_current_floor(void);

void elevator_update_orders(void);

bool elevator_orders_in_order_type(int order_type[]);

bool elevator_orders_exist(void);

int elevator_floor_with_order(void);

bool elevator_check_if_at_floor(void);

int elevator_find_top_floor_with_down_order(void);

int elevator_find_bottom_floor_with_up_order(void);



#endif