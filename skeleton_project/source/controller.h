/**
 * @file controller.h
 * @brief State functions and state machine.
 */


#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "elevator.h"
#include "queue.h"

/**
 * @brief typedef enum defining the possible states and creating type State.
 * 
 */
typedef enum {
	init_state,
	waiting_state,
	move_up_state,
	move_down_state,
	at_floor_state,
	stop_state,
} State;

/**
 * @brief global variable storing the current state.
 * 
 */
State current_state;

/**
 * @brief Function used in waiting state. Updates orders, and decides moving_up or moving_down states when an order is called.
 * 
 */
void controller_decide_up_or_down_state(void);

/**
 * @brief Moves up until the elevator reaches a floor with an order. Up and cab orders are prioritized over down order, and if there are multiple down orders, the elevator goes to the top down order.
 * 
 */
void controller_moving_up(void);

/**
 * @brief Moves down until the elevator reaches a floor with an order. Down and cab orders are prioritized over up order, and if there are multiple up orders, the elevator goes to the bottom up order.
 * 
 * 
 */
void controller_moving_down(void);

/**
 * @brief Function used in at floor state. Opens doors for three seconds while checking stop button and obstruction. Then decides between up-, down- og waiting state.
 * 
 */
void controller_at_floor(void);

/**
 * @brief Function used in stop state. Stops elevator and opens door for three seconds if elevator is on a floor. Deletes orders and clears order lights. Changes current state to waiting state.
 * 
 */
void controller_stop_button(void);

/**
 * @brief Switches between elevator states. 
 * 
 */
void controller_state_machine(void);

#endif
