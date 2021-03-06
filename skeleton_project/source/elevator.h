#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include "hardware.h"
#include "time.h"

int current_floor;
int next_floor;

typedef enum {
	up,
	down
} elevator_direction;


elevator_direction direction; 

int up_orders[4];
int cab_orders[4];
int down_orders[4];



void elevator_clear_all_order_lights(void);

void elevator_reset_all_orders(void);

bool elevator_check_if_at_floor(void);

void elevator_update_orders(void);

bool elevator_check_for_orders(int order_type[]);

bool elevator_orders_exist(void);

int elevator_floor_with_order();

int elevator_closest_floor_with_order();




#endif