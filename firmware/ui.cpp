#include <Arduino.h>
extern "C" {
#include <cont.h>
}
#include "ui.h"
#include "ir_control.h"
#include "buttons.h"
#include "frame_buffer.h"

// to make the world simple, I choosed to use continuation.
// it's a bit itchy that cont_t uses 2kB of RAM, it's too large for
// ui coroutine.
// CONT_STACKSIZE was (SDK's default) 4kB, now shrinked to 2kB.
static cont_t cont;

static void ui_yield()
{
	cont_yield(&cont);
}



//! led test ui
static void ui_led_test()
{
	int x = -1;
	int y = -1;
	for(;;)
	{
		ui_yield();


		uint32_t buttons = button_get();

		if(buttons & BUTTON_OK)
		{
			// ok button; return
			return;
		}

		if(buttons & BUTTON_CANCEL)
		{
			// fill framebuffer with 0xff
			get_current_frame_buffer().fill(0xff);
		}

		if(buttons & (BUTTON_LEFT|BUTTON_RIGHT))
		{
			// vertical line test
			if(buttons & BUTTON_LEFT) --x;
			if(buttons & BUTTON_RIGHT) ++x;
			if(x < 0) x = 0;
			if(x >= LED_MAX_LOGICAL_COL) x = LED_MAX_LOGICAL_COL - 1;
			get_current_frame_buffer().fill(0);
			get_current_frame_buffer().fill(x, 0, 1, LED_MAX_LOGICAL_ROW, 0xff);
		}

		if(buttons &  (BUTTON_UP|BUTTON_DOWN))
		{
			// horizontal line test
			if(buttons & BUTTON_UP) --y;
			if(buttons & BUTTON_DOWN) ++y;
			if(y < 0) y = 0;
			if(y >= LED_MAX_LOGICAL_ROW) y = LED_MAX_LOGICAL_ROW - 1;
			get_current_frame_buffer().fill(0);
			get_current_frame_buffer().fill(0, y, LED_MAX_LOGICAL_COL, 1, 0xff);
		}

	}
}


static void ui_loop()
{
	for(;;)
	{
		ui_yield();

		ui_led_test();
/*
		if(buttons[BUTTON_OK])
		{
			Serial.println("BUTTON_OK");
			buttons[BUTTON_OK] = 0;
			ir_record();
		}

		if(buttons[BUTTON_CANCEL])
		{
			Serial.println("BUTTON_CANCEL");
			buttons[BUTTON_CANCEL] = 0;
			ir_replay();
		}

		if(buttons[BUTTON_UP])
		{
			Serial.println("BUTTON_UP");
			buttons[BUTTON_UP] = 0;

		  double temperature, humidity, pressure;
		  uint8_t measuring, im_update;
		  char s[64];
		  bme280.getData(&temperature, &humidity, &pressure);
		int temp = temperature * 10;
		int hum = humidity;
		int pre = pressure;
		 
		  sprintf(s, "Temperature: %d.%d C, Humidity: %d %%, Pressure: %d hPa\r\n",
				  temp / 10, temp%10, hum, pre);
		  Serial.print(s);

		}

		if(buttons[BUTTON_DOWN])
		{
			Serial.println("BUTTON_DOWN");
			buttons[BUTTON_DOWN] = 0;
			int n = analogRead(0);
			Serial.printf("ambient : %d\r\n", n);
		}
*/
	}
}


void ui_setup()
{
	cont_init(&cont);
}

void ui_process()
{
	cont_run(&cont, &ui_loop);
}
