#include <string.h>
#include "minirisc.h"
#include "harvey_platform.h"
#include "xprintf.h"

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

static uint32_t frame_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];
volatile uint32_t color = 0x00ff0000;


void init_video()
{
	memset(frame_buffer, 0, sizeof(frame_buffer)); // clear frame buffer to black
	VIDEO->WIDTH  = SCREEN_WIDTH;
	VIDEO->HEIGHT = SCREEN_HEIGHT;
	VIDEO->DMA_ADDR = frame_buffer;
	VIDEO->CR = VIDEO_CR_IE | VIDEO_CR_EN;
}


/* Hue must be between 0 and 1536 not included */
uint32_t hue_to_color(unsigned int hue)
{
	uint32_t r, g, b;
	if (hue < 256) {
		r = 255;
		g = hue;
		b = 0;
	} else if (hue < 512) {
		r = 511 - hue;
		g = 255;
		b = 0;
	} else if (hue < 768) {
		r = 0;
		g = 255;
		b = hue - 512;
	} else if (hue < 1024) {
		r = 0;
		g = 1023 - hue;
		b = 255;
	} else if (hue < 1280) {
		r = hue - 1024;
		g = 0;
		b = 255;
	} else if (hue < 1536) {
		r = 255;
		g = 0;
		b = 1535 - hue;
	} else {
		r = 0;
		g = 0;
		b = 0;
	}
	return (r << 16) | (g << 8) | b;
}


void video_interrupt_handler()
{
	static unsigned int hue = 0;
	VIDEO->SR = 0;
	color = hue_to_color(hue);
	hue += 7;
	if (hue >= 1536) {
		hue -= 1536;
	}
}


void draw_square(int x, int y, int width, uint32_t color)
{
	if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
		return;
	}
	int i, j;
	int x_start = x < 0 ? 0 : x;
	int y_start = y < 0 ? 0 : y;
	int x_end = x + width;
	int y_end = y + width;
	if (x_end > SCREEN_WIDTH) {
		x_end = SCREEN_WIDTH;
	}
	if (y_end > SCREEN_HEIGHT) {
		y_end = SCREEN_HEIGHT;
	}
	for (j = y_start; j < y_end; j++) {
		for (i = x_start; i < x_end; i++) {
			frame_buffer[j*SCREEN_WIDTH + i] = color;
		}
	}
}


void draw_disk(int x, int y, int diam, uint32_t color)
{
	if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
		return;
	}
	int i, j;
	int x_start = x < 0 ? 0 : x;
	int y_start = y < 0 ? 0 : y;
	int x_end = x + diam;
	int y_end = y + diam;
	int rad2 = diam*diam/4;
	int xc = x + diam / 2;
	int yc = y + diam / 2;
	if (x_end > SCREEN_WIDTH) {
		x_end = SCREEN_WIDTH;
	}
	if (y_end > SCREEN_HEIGHT) {
		y_end = SCREEN_HEIGHT;
	}
	for (j = y_start; j < y_end; j++) {
		int j2 = (yc - j)*(yc - j);
		for (i = x_start; i < x_end; i++) {
			int i2 = (xc - i)*(xc - i);
			if (j2 + i2 <= rad2) {
				frame_buffer[j*SCREEN_WIDTH + i] = color;
			}
		}
	}
}


volatile int mouse_x = 0;
volatile int mouse_y = 0;
volatile int mouse_draw = 0;
volatile int brush_radius = 3;

void mouse_interrupt_handler()
{
	static int left_button_is_pressed = 0;
	mouse_data_t mouse_event;
	while (MOUSE->SR & MOUSE_SR_FIFO_NOT_EMPTY) {
		mouse_event = MOUSE->DATA;
		switch (mouse_event.type) {
			case MOUSE_MOTION:
				if (left_button_is_pressed) {
					mouse_x = mouse_event.x;
					mouse_y = mouse_event.y;
					mouse_draw = 1;
				}
				break;
			case MOUSE_BUTTON_LEFT_DOWN:
				left_button_is_pressed = 1;
				mouse_x = mouse_event.x;
				mouse_y = mouse_event.y;
				mouse_draw = 1;
				break;
			case MOUSE_BUTTON_LEFT_UP:
				left_button_is_pressed = 0;
				break;
			case MOUSE_WHEEL:
				brush_radius += mouse_event.amount_y;
				if (brush_radius < 1) {
					brush_radius = 1;
				} else if (brush_radius > 64) {
					brush_radius = 64;
				}
				break;
		}
	}
}


void keyboard_interrupt_handler()
{
	uint32_t kdata;
	while (KEYBOARD->SR & KEYBOARD_SR_FIFO_NOT_EMPTY) {
		kdata = KEYBOARD->DATA;
		if (kdata & KEYBOARD_DATA_PRESSED) {
			xprintf("key code: %d\n", KEYBOARD_KEY_CODE(kdata));
			switch (KEYBOARD_KEY_CODE(kdata)) {
				case 113: // Q
					minirisc_halt();
					break;
				case 32: // space
					memset(frame_buffer, 0, sizeof(frame_buffer)); // clear frame buffer
					break;
			}
		}
	}
}


int main()
{
	init_video();

	MOUSE->CR |= MOUSE_CR_IE;
	KEYBOARD->CR |= KEYBOARD_CR_IE;

	minirisc_enable_interrupt(VIDEO_INTERRUPT | MOUSE_INTERRUPT | KEYBOARD_INTERRUPT);

	minirisc_enable_global_interrupts();

	while (1) {
		minirisc_wait_for_interrupt();
		if (mouse_draw) {
			int br = brush_radius;
			draw_disk(mouse_x - br, mouse_y - br, br*2+1, color);
			mouse_draw = 0;
		}
	}

	return 0;
}


