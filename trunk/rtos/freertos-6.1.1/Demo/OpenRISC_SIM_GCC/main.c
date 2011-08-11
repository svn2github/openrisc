
/*
    FreeRTOS V6.1.1 - Copyright (C) 2011 Real Time Engineers Ltd.

    ***************************************************************************
    *                                                                         *
    * If you are:                                                             *
    *                                                                         *
    *    + New to FreeRTOS,                                                   *
    *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
    *    + Looking for basic training,                                        *
    *    + Wanting to improve your FreeRTOS skills and productivity           *
    *                                                                         *
    * then take a look at the FreeRTOS books - available as PDF or paperback  *
    *                                                                         *
    *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
    *                  http://www.FreeRTOS.org/Documentation                  *
    *                                                                         *
    * A pdf reference manual is also available.  Both are usually delivered   *
    * to your inbox within 20 minutes to two hours when purchased between 8am *
    * and 8pm GMT (although please allow up to 24 hours in case of            *
    * exceptional circumstances).  Thank you for your support!                *
    *                                                                         *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public 
    License and the FreeRTOS license exception along with FreeRTOS; if not it 
    can be viewed here: http://www.freertos.org/a00114.html and also obtained 
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

#include <stdlib.h>
#include <string.h>

/* Architecture specific header files. */
#include "spr_defs.h"

/* Scheduler header files. */
#include "FreeRTOS.h"
#include "task.h"

#include "uart.h"
#include "interrupts.h"

#define TASK_STACK_SIZE	(configMINIMAL_STACK_SIZE)

static void prvSetupHardware(void);
void vDemoTask(void *pvParameters);
void vClockTask(void *pvParameters);

void vApplicationTickHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName);

int main(int argc, char **argv) {
	argc = argc;
	argv = argv;

	portBASE_TYPE delay[3] = {600, 400, 200};

	prvSetupHardware();

	xTaskCreate(vDemoTask , (signed char *)"vTask0", TASK_STACK_SIZE, (void*)&delay[0], 1, (xTaskHandle)NULL);	
	xTaskCreate(vDemoTask , (signed char *)"vTask1", TASK_STACK_SIZE, (void*)&delay[1], 2, (xTaskHandle)NULL);	
	xTaskCreate(vDemoTask , (signed char *)"vTask2", TASK_STACK_SIZE, (void*)&delay[2], 3, (xTaskHandle)NULL);	
	xTaskCreate(vClockTask, (signed char *)"vClock", TASK_STACK_SIZE, (void*)&delay[2], 3, (xTaskHandle)NULL);	
		
	vTaskStartScheduler();

	// Control will not reach here
	return 0;
}

void vClockTask(void *pvParameters) {
	pvParameters = pvParameters;
	unsigned portBASE_TYPE ticks = xTaskGetTickCount();
	
	int hour = 0;
	int min = 0;
	int sec = -1;
	while(1) {
		sec += 1;
		if(sec == 60) {
			sec = 0;
			min += 1;
		}
		if(min == 60) {
			min = 0;
			hour += 1;
		}
		if(hour == 24) {
			hour = 0;
		}

		portENTER_CRITICAL();
		{
			uart_print_int(hour);
			uart_print_str(" : ");
			uart_print_int(min);
			uart_print_str(" : ");
			uart_print_int(sec);
			uart_print_str(" , ");
			uart_print_int(ticks);
			uart_print_str("\n\r");
		}
		portEXIT_CRITICAL();

		vTaskDelay(1000);
		ticks = xTaskGetTickCount();
	}
}

void vDemoTask(void *pvParameters) {
	// unsigned portBASE_TYPE priority = uxTaskPriorityGet(NULL);
	unsigned portBASE_TYPE ticks = xTaskGetTickCount();
	portTickType delay = *((portTickType *)pvParameters);

	while(1) {
		portENTER_CRITICAL();
		{
			uart_print_str("vTask ");
			uart_print_int(delay);
			uart_print_str(" : ");
			// uart_print_int(priority);
			// uart_print_str(" , ");
			uart_print_int(ticks);
			uart_print_str(" \n\r");	
		}
		portEXIT_CRITICAL();
		vTaskDelay(delay);
		ticks = xTaskGetTickCount();
	}
}

static void prvSetupHardware(void) {
	// UART controller use 25 Mhz Wishbone bus clock, define in board.h
	uart_init();			

	// Initialize internal Programmable Interrupt Controller
	int_init();
}

void vApplicationTickHook(void) {
	uart_print_str(".");
}

void vApplicationIdleHook(void) {
	uart_print_str(" i ");
}

void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	pxTask = pxTask;
	pcTaskName = pcTaskName;

	uart_print_str(" S \n\r");
}
