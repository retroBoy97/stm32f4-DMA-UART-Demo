#include <stdio.h>
#include "UART.h"
#include "DMA.h"

#define BUFFER_SIZE 5

uint16_t sensor_data_arr[BUFFER_SIZE] = {1, 2, 3, 4, 5};
uint16_t temp_data_arr[BUFFER_SIZE];

volatile uint8_t global_transfer_complete;

int main(void){
    global_transfer_complete = 0;

    UART2_tx_init();

    DMA2_Mem2Mem_Config();
    DMA2_Transfer_Start((uint32_t)sensor_data_arr, (uint32_t)temp_data_arr, (uint32_t)BUFFER_SIZE);

    // Wait until transfer complete
    while(!global_transfer_complete){}
    global_transfer_complete = 0;

    for(int i = 0; i < BUFFER_SIZE; i++){
	printf("temp buffer[%d]: %d \r\n", i, temp_data_arr[i]);
    }

    while(1){

    }
}

void DMA2_Stream0_IRQHandler(void){
    // Check if Transfer Complete
    if((DMA2 -> LISR) & LISR_TCIF0){
	global_transfer_complete = 1;

	// Clear flag
	DMA2 -> LIFCR |= LIFCR_CTCIF0;
    }

    // Check if Transfer transfer error occured
    if((DMA2 -> LISR) & LISR_TEIF0){
	// Do something

	// Clear flag
	DMA2 -> LIFCR |= LIFCR_CTEIF0;
    }
}

