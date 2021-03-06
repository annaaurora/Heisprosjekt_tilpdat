#include "elevator.h"


/*OVERKILL, laget en myyye enklere*/
int order_array(){
	int k = 0;
	int order_array[10][3]; /*max antall kommandoer er 10, int etasje og int order_type*/
	for(int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; i++){
		for(int HardwareMovement = HARDWARE_ORDER_UP; HardwareMovement <= HARDWARE_ORDER_UP; HardwareMovement){
			if(hardware_read_order(i, HardwareMovement)){
				order_array[k][0] = 1;
				order_array[k][1] = i;
				order_array[k][2] = HardwareMovement;
				k += 1;
			}
			else if((i == 0 && HardwareMovement == HARDWARE_ORDER_DOWN) || (i==3 && HardwareMovement == HARDWARE_ORDER_UP)){
					k=k;
			}
			else{ 
				order_array[k][0] = 0;
				order_array[k][1] = i;
				order_array[k][2] = HardwareMovement; 
				k += 1;
			}
		}
	}
	int n = 0;
	for(int i = 0; i < (sizeof(order_array)/sizeof(order_array[0])); i++){
		if(order_array[i][0] =! 0){
			n +=1;
		}
	}

	int arranged_order_array[k][2];

	for(int l = 0; l < k; l++){
			for(int j = 0; j < (sizeof(order_array)/sizeof(order_array[0])); j++){
				if(order_array[j][0] =! 0){
					arranged_order_array[l][0] = order_array[j][1];
					arranged_order_array[l][1] = order_array[j][2];
				}
			}
	}
	return arranged_order_array;
}

/*basically en sleep-funksjon ... :-)*/

void timer(){
	
	int start_time = (clock() * 1000)/CLOCKS_PER_SEC;
	
	int end_time = start_time + 3000;

	do {
		start_time = (clock() * 1000)/CLOCKS_PER_SEC;
		/*if(hardware_read_obstruction_signal()){
			end_time = start_time + 3000; gjør timeren 2 sek tregere*/
	} while(start_time <= end_time);
}
