/**
 * @file 
 * @brief Elevator functions
 */


#ifndef ELEVATOR_H
#define ELEVATOR_H


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include "hardware.h"
#include "time.h"

/**
 * @brief global variable storing current floor.
 * 
 */
int current_floor;

/**
 * @brief typedef enum defining possible directions and creating type elevator_direction.
 * 
 */
typedef enum {
	up,
	down
} elevator_direction;

/**
 * @brief global variable storing direction.
 * 
 */
elevator_direction direction; 

/**
 * @brief clearing all order ligths.
 * 
 */
void elevator_clear_all_order_lights(void);

/**
 * @brief returns true if elevator is on a floor.
 * 
 * @return true 
 * @return false 
 */
bool elevator_check_if_at_floor(void);

#endif