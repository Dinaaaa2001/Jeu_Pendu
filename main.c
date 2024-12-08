#include "minirisc.h"
#include "harvey_platform.h"
#include "xprintf.h"
#include "FreeRTOS.h"
#include "task.h"


void hello_task(void *arg)
{
	(void)arg;
	while (1) {
		xprintf("Hello, World!\n");
		vTaskDelay(MS2TICKS(1000));
	}
}

int main()
{
	xTaskCreate(hello_task, "hello", 1024, NULL, 1, NULL);
	vTaskStartScheduler();

	return 0;
}


