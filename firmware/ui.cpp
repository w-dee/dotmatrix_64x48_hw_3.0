#include <Arduino.h>
#include <vector>
#include <algorithm>
#include <Ticker.h>
#include "ui.h"
#include "ir_control.h"
#include "buttons.h"
#include "frame_buffer.h"
#include "matrix_drive.h"
#include "wifi.h"

// undefining min and max are needed to include <vector>
#undef min
#undef max
#include <vector>
#include <functional>

#include "fonts/font_5x5.h"
#include "fonts/font_bff.h"
#include "fonts/font_aa.h"


typedef std::vector<String> string_vector;

enum transition_t { t_none };


class screen_base_t
{
public:
	//! The constructor
	screen_base_t() {;}

	//! The destructor
	virtual ~screen_base_t() {;}

protected:
	//! Called when a button is pushed
	virtual void on_button(uint32_t button) {;}

	//! Repeatedly called 10ms intervally when the screen is active
	virtual void on_idle_10() {;}

	//! Repeatedly called 50ms intervally when the screen is active
	virtual void on_idle_50() {;}

	//! Draw content; this function is automatically called 20ms
	//! intervally to refresh the content
	virtual void draw() {;}

	//! Call this when the screen content is written and need to be showed
	void show(transition_t transition = t_none);

	//! Call this when the screen needs to be closed
	void close();

	//! Short cut to background frame buffer
	static frame_buffer_t & fb() { return get_bg_frame_buffer(); }

	//! Call this to get cursor blink intensity
	//! which is automatically increasing then decreasing repeatedly
	static uint8_t get_blink_intensity();

	//! Call this to reset cursor blink intensity
	static void reset_blink_intensity();

private:

	friend class screen_manager_t;
};

class screen_manager_t
{
	transition_t transition; //!< current running transition
	bool in_transition; //! whether the transition is under progress
	std::vector<screen_base_t *> stack;
	bool stack_changed;
	int8_t tick_interval_50 = 0; // to count 10ms tick to process 50ms things
	int8_t tick_interval_20 = 0; // to count 10ms tick to process 20ms things
	int8_t blink_intensity = 0; //!< cursor blink intensity value

public:
	screen_manager_t()
	{
		transition = t_none; in_transition = false; stack_changed = false;
	}

	void show(transition_t tran)
	{
		transition = tran; // TODO: transition handling
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

public:
	void process()
	{
		size_t sz = stack.size();
		if(sz)
		{
			if(!in_transition) // TODO: transition handling
			{
				stack_changed = false;
				screen_base_t *top = stack[sz -1];

				// dispatch button event
				uint32_t buttons = button_get();
				for(uint32_t i = 1; i; i <<= 1)
					if(buttons & i) top->on_button(i);

				// care must be taken,
				// the screen may be poped (removed) during button event
				if(!stack_changed && tick_interval_20 == 0)
				{
					// dispatch draw event
					top->draw();
				}

				// care must be taken again
				if(!stack_changed)
				{
					// dispatch idle event
					top->on_idle_10();
				}

				// care must be taken again
				if(!stack_changed && tick_interval_50 == 0)
				{
					// dispatch idle event
					top->on_idle_50();
				}
			}
		}
		++tick_interval_50;
		if(tick_interval_50 == 5) { tick_interval_50 = 0; blink_intensity += 21; }
		++tick_interval_20;
		if(tick_interval_20 == 5) { tick_interval_20 = 0; }
	}


	/**
	 * Get cursor blink intensity
	 */
	uint8_t get_blink_intensity() const
	{
		int i = blink_intensity<0 ? -blink_intensity : blink_intensity;
		i <<= 1;
		if(i > 255) i = 255;
		return (int8_t) i;
	}

	/**
	 * Reset cursor blink intensity
	 */
	void reset_blink_intensity()
	{
		blink_intensity = 128;
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

uint8_t screen_base_t::get_blink_intensity()
{
	return screen_manager.get_blink_intensity();
}

void screen_base_t::reset_blink_intensity()
{
	screen_manager.reset_blink_intensity();
}


//! Simple message box
class screen_message_box_t : public screen_base_t
{
	static constexpr int char_list_start_y = 7+6; // character list start position y in pixel

	String title;
	string_vector lines;

public:
	screen_message_box_t(const String & _title, const string_vector & _lines) :
		title(_title), lines(_lines)
	{
	}

	// TODO: scroll and text formatting

protected:
	void draw() override
	{
		// erase
		fb().fill(0, 0, LED_MAX_LOGICAL_COL, LED_MAX_LOGICAL_ROW, 0);

		// draw title
		fb().draw_text(0, 0, 255, title.c_str(), font_5x5);

		// draw line
		fb().fill(0, 7, LED_MAX_LOGICAL_COL, 1, 128);

		// draw char_list
		for(size_t i = 0; i < lines.size(); ++i)
		{
			fb().draw_text(0, i*6+char_list_start_y,
					255, lines[i].c_str(), font_5x5);
		}

		// show the drawn content
		show();
	}

	void on_button(uint32_t button) override
	{
		switch(button)
		{
		case BUTTON_OK:
		case BUTTON_CANCEL:
			screen_manager.pop();
			break;
		}
	}

};

//! led test ui
class screen_led_test_t : public screen_base_t
{
	
	int x = -1;
	int y = -1;

protected:
	void on_button(uint32_t button) override
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
protected:

	static constexpr int num_w_chars = 10; //!< maximum chars in a horizontal line
	static constexpr int num_char_list_display_lines = 6; //!< maximum display-able lines of char list
	static constexpr int char_list_start_y = 7+6; // character list start position y in pixel

	String title; //!< the title
	String line; //!< a string to be edited
	int max_chars; //!< maximum bytes arrowed

	string_vector char_list;

	int line_start = 0; //!< line display start character index
	int char_list_start = 0; //!< char_list display start line index
	int cursor = 0; //!< cursor position in line
	int y = 0; //!< logical position in char_list or line; 0=line, 1=BS/DEL, 2~ = char_list
	int x = 0; //!< logical position in char_list
	int px = 0; //!< physical x position (where cursor blinks)

public:

	screen_ascii_editor_t(const String &_title, const String &_line = "", int _max_chars = -1) :
			title(_title),
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
	}

protected:
	void draw() override
	{
		// erase
		fb().fill(0, 0, LED_MAX_LOGICAL_COL, LED_MAX_LOGICAL_ROW, 0);

		// draw title
		fb().draw_text(0, 0, 255, title.c_str(), font_5x5);

		// draw line
		fb().fill(0, 7, LED_MAX_LOGICAL_COL, 1, 128);

		// draw line cursor
		fb().fill((cursor - line_start) * 6, 7, 1, 5, get_blink_intensity()); 

		// draw block cursor
		if(y == 1)
		{
			// BS or DEL
			if(x <= 2)
				fb().fill(0, char_list_start_y, 6*2, char_list_start_y, get_blink_intensity()); // BS
			else
				fb().fill(6*3, char_list_start_y, 6*3, char_list_start_y,  get_blink_intensity()); // DEL
		}
		else if(y >= 2)
		{
			// in char list
			fb().fill(px*6, (y - char_list_start - 1)*6 + char_list_start_y, 5, 5,
				get_blink_intensity());
		}

		// draw edit line
		fb().draw_text(1, 7, 255, line.c_str() + line_start, font_5x5);

		// draw char_list
		for(int i = 0; i < num_char_list_display_lines; ++i)
		{
			if(i+char_list_start < char_list.size())
			{
				fb().draw_text(0, i*6+char_list_start_y,
					255, char_list[i+char_list_start].c_str(), font_5x5);
			}
		}

		// show the drawn content
		show();
	}

	/**
	 * Justify edit line text display range
	 */
	void adjust_edit_line()
	{
		if(line_start + num_w_chars - 1 <= cursor)
		{
			line_start = cursor - (num_w_chars - 1) + 1;
			int line_length = line.length();
			if(line_start + num_w_chars > line_length)
				line_start = line_length - num_w_chars;
		}
		if(line_start + 2 > cursor)
		{
			line_start = cursor - 2;
			if(line_start < 0) line_start = 0;
		}
	}

	/**
	 * Justify physical x
	 */
	void adjust_px()
	{
		px = x;
		if(y >= 2)
		{
			int len = char_list[y-1].length();
			if(px > len - 1) px = len - 1; 
		}
	}

	/**
	 * Justify char list vertical range
	 */
	void adjust_char_list_range()
	{
		if(y == 0) return;
		int ny = y - 1;

		if(ny < char_list_start) char_list_start = ny;
		else if(ny >= char_list_start + num_char_list_display_lines)
			char_list_start = ny - num_char_list_display_lines + 1;
	}

	/**
	 * Button handler
	 */
	void on_button(uint32_t button) override
	{
		reset_blink_intensity();
		switch(button)
		{
		case BUTTON_LEFT:
			if(y == 0)
			{
				// edit line
				if(cursor > 0) --cursor;
				adjust_edit_line();
			}
			else if(y == 1)
			{
				// BS or DEL
				x = 0; // BS
			}
			else
			{
				// in char list
				if(x > 0) --x;
				adjust_px();
			}
			break;

		case BUTTON_RIGHT:
			if(y == 0)
			{
				// edit line
				if(cursor < line.length()) ++ cursor;
				adjust_edit_line();
			}
			else if(y == 1)
			{
				// BS or DEL
				x = 3; // DEL
			}
			else
			{
				// in char list
				++ x;
				adjust_px();
				x = px;
			}
			break;

		case BUTTON_UP:
			if(y > 0) --y;
			adjust_px();
			adjust_char_list_range();
			break;

		case BUTTON_DOWN:
			if(y < char_list.size() + 1 - 1) ++y;
			adjust_px();
			adjust_char_list_range();
			break;

		case BUTTON_OK:
			if(y == 0)
			{
				// ok, return
				on_ok(line);
			}
			else if(y == 1)
			{
				// BS or DEL
				if(x <= 2)
				{
					// BS
					if(cursor > 0)
					{
						String pline = line;
						line = pline.substring(0, cursor - 1) +
							pline.substring(cursor, pline.length());
						--cursor;
					}
				}
				else
				{
					// DEL
					if(line.length() >= cursor + 1)
					{
						String pline = line;
						line = pline.substring(0, cursor) +
								pline.substring(cursor + 1, pline.length());
					}
				}
				adjust_edit_line();
			}
			else
			{
				if(line.length() < max_chars)
				{
					// insert a char
					line = line.substring(0, cursor) + char_list[y-1][px] +
						line.substring(cursor, line.length());
					++cursor;
				}
				adjust_edit_line();
			}
			break;

		case BUTTON_CANCEL:
			on_cancel();
			break;
		}
	}

	virtual void on_ok(const String & line) {;} //!< when the user pressed OK butotn
	virtual void on_cancel() {;} //!< when the user pressed CANCEL butotn

};


//! IP string editor UI
class screen_ip_editor_t : public screen_ascii_editor_t
{
public:
	screen_ip_editor_t(const String & _title, const String &_line = "") :
		screen_ascii_editor_t(_title, _line, 15) // XXX.XXX.XXX.XXX = 15 chars
	{
		char_list = { F("BS DEL"), F("56789."), F("01234") };
	}
};

//! Menu list UI
class screen_menu_t : public screen_base_t
{
protected:
	String title;
	string_vector items;

	static constexpr int max_lines = 6; //!< maximum item lines per a screen
	int x = 0; //!< left most column to be displayed
	int y = 0; //!< selected item index
	int y_top = 0; //!< display start index at top line of the items
	bool h_scroll = false; //!< whether to allow horizontal scroll

public:
	screen_menu_t(const String &_title, const string_vector & _items) :
		 title(_title), items(_items)
	{
	}

	void set_h_scroll(bool b)
	{
		h_scroll = b;
		if(!h_scroll) x = 0;
	}

protected:
	void draw() override
	{
		// erase
		fb().fill(0, 0, LED_MAX_LOGICAL_COL, LED_MAX_LOGICAL_ROW, 0);

		// draw the title
		fb().draw_text(0, 0, 255, title.c_str(), font_5x5);

		// draw line
		fb().fill(0, 7, LED_MAX_LOGICAL_COL, 1, 128);

		// draw cursor
		fb().fill(2, (y - y_top)*6 + 8, LED_MAX_LOGICAL_COL - 2, 5, get_blink_intensity());

		// draw items
		for(int i = 0; i < max_lines; ++ i)
		{
			if(i + y_top < items.size())
			{
				if(x < items[i + y_top].length() - 1)
					fb().draw_text(1, i * 6 + 8, 255, items[i + y_top].c_str() + x, font_5x5);
			}
		}

		// show the drawn content
		show();
	}

	void on_button(uint32_t button) override
	{
		reset_blink_intensity();
		switch(button)
		{
		case BUTTON_UP:
			if(y > 0) --y;
			if(y_top > y) y_top = y;
			break;

		case BUTTON_DOWN:
			if(y < items.size() - 1) ++y;
			if(y_top < y - (max_lines - 1)) y_top = y - (max_lines - 1);
			break;

		case BUTTON_LEFT:
			if(h_scroll && x > 0) --x;
			break;

		case BUTTON_RIGHT:
			if(h_scroll) ++x;
			break;

		case BUTTON_OK:
			on_ok(y);
			break;

		case BUTTON_CANCEL:
			on_cancel();
			break;
		}
	}

	virtual void on_ok(int item_idx) {;} //!< when the user pressed OK butotn
	virtual void on_cancel() {;} //!< when the user pressed CANCEL butotn

};

class screen_dns_2_editor_t : public screen_ip_editor_t
{
public:
	screen_dns_2_editor_t() : screen_ip_editor_t(F("DNS Srvr 2"), F("")) {}

protected:
	void on_ok(const String & line) override
	{
		screen_manager.pop();
	} 
};


class screen_dns_1_editor_t : public screen_ip_editor_t
{
public:
	screen_dns_1_editor_t() : screen_ip_editor_t(F("DNS Srvr 1"), F("")) {}

protected:
	void on_ok(const String & line) override
	{
		screen_manager.pop();
		screen_manager.push(new screen_dns_2_editor_t());
	} 
};


class screen_net_mask_editor_t : public screen_ip_editor_t
{
public:
	screen_net_mask_editor_t() : screen_ip_editor_t(F("Net Mask"), F("")) {}

protected:
	void on_ok(const String & line) override
	{
		screen_manager.pop();
		screen_manager.push(new screen_dns_1_editor_t());
	} 
};


class screen_ip_addr_editor_t : public screen_ip_editor_t
{
public:
	screen_ip_addr_editor_t() : screen_ip_editor_t(F("IP Addr"), F("")) {}

protected:
	void on_ok(const String & line) override
	{
		screen_manager.pop();
		screen_manager.push(new screen_net_mask_editor_t());
	} 
};

class screen_dhcp_mode_t : public screen_menu_t
{
public:
	screen_dhcp_mode_t() : screen_menu_t(
				F("DHCP mode"),{
					F("Use DHCP  "),
					F("Manual IP >"),
				} ) {	}

protected:
	void on_ok(int idx) override
	{
		switch(idx)
		{
		case 0: // Use DHCP
			break;

		case 1: // Manual IP
			screen_manager.push(new screen_ip_addr_editor_t());
			break;
		}
	}

	void on_cancel() override
	{
		screen_manager.pop();
	}
};

class screen_ap_list_t : public screen_menu_t
{
	int num_stations; //!< number of stations scaned
public:
	screen_ap_list_t(int _num_stations) :
		screen_menu_t( F("AP List"), {} ),
		num_stations(_num_stations)
	{
		set_h_scroll(true);
		// Push station names order by its rssi.
		// Here we use naiive method to sort the list...
		// Assumes the list is not so big.
		for(int rssi = 0; rssi >= -100; --rssi)
		{
			for(int i = 0; i < num_stations; ++i)
			{
				if(WiFi.RSSI(i) == rssi)
					items.push_back(WiFi.SSID(i));
			}
		}
		// Delete scanned list to release precious heap
		WiFi.scanDelete();
	}

protected:
	void on_cancel() override
	{
		screen_manager.pop();
	}

};

class screen_wifi_scanning_t : public screen_base_t
{
	String line[2];

public:
	screen_wifi_scanning_t() : line { F("Scanning"), F("Networks") }
	{
		WiFi.scanNetworks(/*async=*/true, /*show_hidden=*/false);
	}

	void draw() override
	{
		// erase
		fb().fill(0, 0, LED_MAX_LOGICAL_COL, LED_MAX_LOGICAL_ROW, 0);

		// draw the text
		fb().draw_text(0, 12, get_blink_intensity(), line[0].c_str(), font_5x5);
		fb().draw_text(0, 18, get_blink_intensity(), line[1].c_str(), font_5x5);

		// show the drawn content
		show();
	}

protected:
	void on_idle_50() override
	{
		int state = WiFi.scanComplete();
		if(state > 0)
		{
			// scan complete (or failed)
			screen_manager.pop();
			screen_manager.push(new screen_ap_list_t(state));
		}
		else if(state == 0)
		{
			// scan complete but no APs found
			screen_manager.pop();
			screen_manager.push(new screen_message_box_t(F("AP List"), {F("No APs"), F("Found")}));
		}
	}

};

//				F("\u2082\u2081\u2083\u208f\u2082\u2081\u2083\u208f\u2082\u2081\u2083\u208f\u2082\u2081\u2083"),{

class screen_wifi_setting_t : public screen_menu_t
{
public:
	screen_wifi_setting_t() : screen_menu_t(
				F("WiFi config"),{
					F("WPS       >"),
					F("AP List   >"),
					F("DHCP mode >"),
				} ) {	}

protected:
	void on_ok(int idx) override
	{
		switch(idx)
		{
		case 0: // WPS
			break;

		case 1: // AP List
			screen_manager.push(new screen_wifi_scanning_t());
			break;

		case 2: // DHCP mode
			screen_manager.push(new screen_dhcp_mode_t());
			break;
		}
	}

};



//! channel test ui
class screen_channel_test_t : public screen_base_t
{
	int div = 0;

protected:
	void on_button(uint32_t button) override
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

static void ui_ticker() { screen_manager.process(); }

void ui_setup()
{
	static Ticker ticker;
	ticker.attach_ms(10, &ui_ticker);

//	screen_manager.push(new screen_ascii_editor_t("0123456789qwe"));
//	screen_manager.push(new screen_ip_editor_t("IP addr", "012.88.77.65"));
//	screen_manager.push(new screen_menu_t(F("Test menu"), {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"} ));
	screen_manager.push(new screen_wifi_setting_t());
}

void ui_process()
{

}
