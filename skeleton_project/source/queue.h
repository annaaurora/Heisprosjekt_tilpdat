/**
 * @file queue.h
 * @brief Order functions.
 */

#ifndef QUEUE_H
#define QUEUE_H


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include "hardware.h"
#include "elevator.h"
#include "time.h"

/**
 * @brief arrays storing orders.
 * 
 */
int up_orders[HARDWARE_NUMBER_OF_FLOORS];
int cab_orders[HARDWARE_NUMBER_OF_FLOORS];
int down_orders[HARDWARE_NUMBER_OF_FLOORS];

/**
 * @brief clearing all orders in order arrays.
 * 
 */
void queue_clear_all_orders(void);

/**
 * @brief clears orders in order array only at current floor.
 * 
 */
void queue_clear_orders_at_current_floor(void);

/**
 * @brief updates orders in order arrays and sets order lights.
 * 
 */
void queue_update_orders(void);

/**
 * @brief returns true if there are any orders of param order_type.
 * 
 * @param order_type 
 * @return true 
 * @return false 
 */
bool queue_orders_in_order_type(int order_type[]);

/**
 * @brief returns true if there are any existing orders.
 * 
 * @return true 
 * @return false 
 */
bool queue_orders_exist(void);

/**
 * @brief returns which floor has order. Only used when there is only one order.
 * 
 * @return int 
 */
int queue_floor_with_order(void);

/**
 * @brief returns the top floor that has a down order.
 * 
 * @return int 
 */
int queue_find_top_floor_with_down_order(void);

/**
 * @brief returns the bottom floor that has an up order.
 * 
 * @return int 
 */
int queue_find_bottom_floor_with_up_order(void);




#endif