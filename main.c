#include <stdio.h>
#include <time.h>
#include "uart.h"
#include "FreeRTOS.h"
#include "task.h"


void hello_task(void *arg)
{
	(void)arg;
	while (1) {
		time_t tv = time(NULL);
		printf("Hello, World!  The date is: %s", ctime(&tv));
		vTaskDelay(MS2TICKS(1000));
	}
}


int main()
{
	init_uart();

	xTaskCreate(hello_task, "hello", 1024, NULL, 1, NULL);
	vTaskStartScheduler();

	return 0;
}

