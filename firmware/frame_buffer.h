#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

static constexpr int LED_MAX_LOGICAL_ROW = 48;
static constexpr int LED_MAX_LOGICAL_COL = 64;

class font_base_t;

class frame_buffer_t
{
public:
	typedef unsigned char array_t[LED_MAX_LOGICAL_ROW][LED_MAX_LOGICAL_COL];

private:
	array_t buffer;

public:
	//! returns width
	int get_width() const { return LED_MAX_LOGICAL_COL; }
	//! returns height
	int get_height() const { return LED_MAX_LOGICAL_ROW; }

	//! clip bounding box
	//! returns wheter the box is remaining
	bool clip(int &fx, int &fy, int &x, int &y, int &w, int &h) const;

	//! Returns array
	array_t & ICACHE_RAM_ATTR array() { return buffer; }

	//! Set point at specified intencity level.
	//! Note that this method does not check the boundary.
	void set_point(int x, int y, int level)
	{
		buffer[y][x] = level;
	}
	//! get intencity level at specified point.
	//! Note that this method does not check the boundary.
	int get_point(int x, int y) const
	{
		return buffer[y][x] ;
	}

	//! Draw a character at specified position
	void draw_char(int x, int y, int level, int ch, const font_base_t & font); 

	//! Draw text at specified position
	void draw_text(int x, int y, int level, const __FlashStringHelper *ifsh, const font_base_t & font); 

	void draw_text(int x, int y, int level, const String &s, const font_base_t & font)
	{
			draw_text(x, y, level, s.c_str(), font);
	}

	void draw_text(int x, int y, int level, const char *s, const font_base_t & font);

	//! Get text width specified by the string and font
	int get_text_width(const String &s, const font_base_t & font)
	{
		return get_text_width(s.c_str(), font);
	}

	int get_text_width(const char *s, const font_base_t & font);

	//! fill all region with specified value
	void fill(int level);

	//! fill specified region with specified value
	void fill(int x, int y, int w, int h, int level);
};


// the framebuffer
extern frame_buffer_t buffers[2]; // for double buffering
extern frame_buffer_t & current_frame_buffer;
extern frame_buffer_t & bg_frame_buffer;

static inline ICACHE_RAM_ATTR frame_buffer_t & get_current_frame_buffer() { return current_frame_buffer;}
static inline ICACHE_RAM_ATTR frame_buffer_t & get_bg_frame_buffer() { return bg_frame_buffer;}

//! swap current frame buffer
void frame_buffer_flip();

#endif
