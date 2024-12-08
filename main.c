#include <stdio.h>
#include <string.h>
#include <time.h>
#include "minirisc.h"
#include "uart.h"
#include "FreeRTOS.h"
#include "task.h"


void hello_task(void *arg)
{
	(void)arg;
	time_t tv = time(NULL);

	while (1) {
		tv = time(NULL);
		printf("Hello, World!  The date is: %s", ctime(&tv));
		vTaskDelay(MS2TICKS(1000));
	}
}


void echo_task(void *arg)
{
	(void)arg;
	char buf[128];

	while (1) {
		int i = 0;
		char c;
		while ((c = getchar()) != '\r' && i != (sizeof(buf) - 1)) {
			buf[i++] = c;
		}
		buf[i] = '\0';
		printf("Received: \"%s\"\n", buf);
		if (strcmp(buf, "quit") == 0) {
			minirisc_halt();
		}
	}
}


int main()
{
	init_uart();

	xTaskCreate(hello_task, "hello", 1024, NULL, 1, NULL);
	xTaskCreate(echo_task,  "echo",  1024, NULL, 1, NULL);
	vTaskStartScheduler();

	return 0;
}

