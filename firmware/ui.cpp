#include <Arduino.h>
#include <vector>
#include <algorithm>
#include "ui.h"
#include "ir_control.h"
#include "buttons.h"
#include "frame_buffer.h"
#include "matrix_drive.h"
#include "wifi.h"

#include "fonts/font_5x5.h"
#include "fonts/font_bff.h"
#include "fonts/font_aa.h"

enum transition_t { t_none };


class screen_base_t
{
public:
	//! The constructor
	screen_base_t() {;}

	//! The destructor
	virtual ~screen_base_t() {;}

	//! Called when a button is pushed
	virtual void on_button(uint32_t button) {;}

	//! Repeatedly called 10ms intervally when the screen is active
	virtual void on_idle() {;}

	//! Call this when the screen content is written and need to be showed
	void show(transition_t transition = t_none);

	//! Call this when the screen needs to be closed
	void close();
	

private:

//	friend class screen_manager_t;
};

class screen_manager_t
{
	transition_t transition; //!< current running transition
	bool in_transition; //! whether the transition is under progress
	uint32_t next_check;
	std::vector<screen_base_t *> stack;
	bool stack_changed;

public:
	screen_manager_t() { transition = t_none; in_transition = false; stack_changed = false;}

	void show(transition_t tran)
	{
		transition = tran;
		if(transition == t_none)
		{
			// immediate show
			frame_buffer_flip();
		}
	}

	void push(screen_base_t * screen)
	{
		stack.push_back(screen);
		stack_changed = true;
	}

	void close(screen_base_t * screen)
	{
		std::vector<screen_base_t *>::iterator it =
			std::find(stack.begin(), stack.end(), screen);
		if(it != stack.end())
		{
			stack.erase(it);
			stack_changed = true;
		}
	}

	void pop()
	{
		if(stack.size())
		{
			delete stack[stack.size() - 1];
			stack.pop_back();
			stack_changed = true;
		}
	}

	void process()
	{
		uint32_t now = millis();
		if((int32_t)(now - next_check) > 0)
		{
			size_t sz = stack.size();
			if(sz)
			{
				if(!in_transition)
				{
					stack_changed = false;
					screen_base_t *top = stack[sz -1];

					// dispatch button event
					uint32_t buttons = button_get();
					for(uint32_t i = 1; i; i <<= 1)
						if(buttons & i) top->on_button(i);

					// care must be taken,
					// the screen may be poped (removed) during button event
					if(!stack_changed)
					{
						// dispatch idle event
						top->on_idle();
					}
				}
			}
			next_check += 10;
			if((int32_t)(now - next_check) > 0)
				next_check = now + 10; // still in past; reset next_check
		}
	}
};

static screen_manager_t screen_manager;

void screen_base_t::show(transition_t transition)
{
	screen_manager.show(transition);
}

void screen_base_t::close()
{
	screen_manager.close(this);
}


//! led test ui
class screen_led_test_t : public screen_base_t
{
	
	int x = -1;
	int y = -1;

	void on_button(uint32_t button)
	{
		switch(button)
		{
		case BUTTON_OK:
			// ok button; return
			get_current_frame_buffer().fill(0x00);
			return;

		case BUTTON_CANCEL:
			// fill framebuffer with 0xff
			get_current_frame_buffer().fill(0xff);
			return;

		case BUTTON_LEFT:
		case BUTTON_RIGHT:
			// vertical line test
			if(button == BUTTON_LEFT) --x;
			if(button == BUTTON_RIGHT) ++x;
			if(x < 0) x = 0;
			if(x >= LED_MAX_LOGICAL_COL) x = LED_MAX_LOGICAL_COL - 1;
			get_current_frame_buffer().fill(0);
			get_current_frame_buffer().fill(x, 0, 1, LED_MAX_LOGICAL_ROW, 0xff);
			return;

		case BUTTON_UP:
		case BUTTON_DOWN:
			// horizontal line test
			if(button == BUTTON_UP) --y;
			if(button == BUTTON_DOWN) ++y;
			if(y < 0) y = 0;
			if(y >= LED_MAX_LOGICAL_ROW) y = LED_MAX_LOGICAL_ROW - 1;
			get_current_frame_buffer().fill(0);
			get_current_frame_buffer().fill(0, y, LED_MAX_LOGICAL_COL, 1, 0xff);
			return;
		}

	}
};

//! ASCII string editor UI
class screen_ascii_editor_t : public screen_base_t
{
	static constexpr int num_char_list_lines = 12;

	String line; //!< a string to be edited
	int max_chars; //!< maximum bytes arrowed

	String char_list[num_char_list_lines];

	int line_start = 0; //!< line display start character index
	int char_list_start = 0; //!< char_list display start line index
	int cursor = 0; //!< cursor position in line
	int y = 0; //!< logical position in char_list or line; 0=line, 1=BS/DEL, 2~ = char_list
	int x = 0; //!< logical position in char_list

public:

	screen_ascii_editor_t(const String &_line = "", int _max_chars = -1) :
			line(_line),
			max_chars(_max_chars),
			char_list
				{
					F("BS DEL"),
					F("0123456789"),
					F("qwertyuiop"),
					F("asdfghjkl"),
					F("zxcvbnm"),
					F("QWERTYUIOP"),
					F("ASDFGHJKL"),
					F("ZXCVBNM"),
					F(" !\"#$%&'()"),
					F("*+,-./:;"),
					F("@[\\]^_<=>?"),
					F("`{|}~")
				}

	{
		draw();
	}

	void draw()
	{
		auto && fb = get_current_frame_buffer();
		// erase
		fb.fill(
			0, 0, LED_MAX_LOGICAL_COL, LED_MAX_LOGICAL_ROW, 0);

		// draw line
		fb.draw_text(0, 0, 255, line.c_str() + line_start, font_5x5);

		// draw char_list
		for(int i = 0; i < 8; ++i)
		{
			fb.draw_text(0, i*6, 255, char_list[i+1].c_str(), font_5x5);
		}
	}


	void on_button(uint32_t button)
	{
		switch(button)
		{
		case BUTTON_OK:
			wifi_wps();
			break;
		}
	}

};


//	get_current_frame_buffer().draw_text(0, 0, 255, "0123", font_large_digits);
//	get_current_frame_buffer().draw_text(0, 12, 255, "b", font_bff);
//	get_current_frame_buffer().draw_text(0, 24, 255, "c", font_bff);
//	get_current_frame_buffer().draw_text(0, 36, 255, "　-dee", font_bff);


//! channel test ui
class screen_channel_test_t : public screen_base_t
{
	int div = 0;

	void on_button(uint32_t button)
	{
		switch(button)
		{
		case BUTTON_OK:
			// ok button; return
			return;

		case BUTTON_CANCEL:
			// fill framebuffer with 0xff
			return;

		case BUTTON_LEFT:
		case BUTTON_RIGHT:
			// vertical line test
			if(button == BUTTON_LEFT) --div;
			if(button == BUTTON_RIGHT) ++div;
			if(div < 0) div = 0;
			if(div >= LED_NUM_INTERVAL_MODE) div = LED_NUM_INTERVAL_MODE - 1;
			led_set_interval_mode(div);
			get_current_frame_buffer().fill(0, 0, 5, 1, 0);
			get_current_frame_buffer().fill(0, 0, div + 1, 1, 0xff);
			return;

		case BUTTON_UP:
		case BUTTON_DOWN:
			return;
		}

	}
};

#if 0
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
#endif

void ui_setup()
{
	screen_manager.push(new screen_ascii_editor_t());
}

void ui_process()
{
	screen_manager.process();
}
