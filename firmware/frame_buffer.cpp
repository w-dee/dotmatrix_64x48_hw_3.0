#include <Arduino.h>
#include "frame_buffer.h"
#include "fonts/font.h"

frame_buffer_t buffers[2]; // for double buffering
frame_buffer_t & current_frame_buffer = buffers[0];
frame_buffer_t & bg_frame_buffer = buffers[1];

static int get_utf8_bytes(uint8_t c)
{
	if(c < 0x80) return 1;
	if(c < 0xc2) return 1; // invalid
	if(c < 0xe0) return 2;
	if(c < 0xf0) return 3;
	if(c < 0xf8) return 4;
	if(c < 0xfc) return 5;
	if(c < 0xfe) return 6;
	return 1; // invalid

}

static bool utf8tow(const uint8_t * & in, uint32_t *out)
{
	// convert a utf-8 charater from 'in' to wide charater 'out'
	const uint8_t * p = (const uint8_t * &)in;
	if(p[0] < 0x80)
	{
		if(out) *out = (uint32_t)in[0];
		in++;
		return true;
	}
	else if(p[0] < 0xc2)
	{
		// invalid character
		return false;
	}
	else if(p[0] < 0xe0)
	{
		// two bytes (11bits)
		if((p[1] & 0xc0) != 0x80) return false;
		if(out) *out = ((p[0] & 0x1f) << 6) + (p[1] & 0x3f);
		in += 2;
		return true;
	}
	else if(p[0] < 0xf0)
	{
		// three bytes (16bits)
		if((p[1] & 0xc0) != 0x80) return false;
		if((p[2] & 0xc0) != 0x80) return false;
		if(out) *out = ((p[0] & 0x1f) << 12) + ((p[1] & 0x3f) << 6) + (p[2] & 0x3f);
		in += 3;
		return true;
	}
	else if(p[0] < 0xf8)
	{
		// four bytes (21bits)
		if((p[1] & 0xc0) != 0x80) return false;
		if((p[2] & 0xc0) != 0x80) return false;
		if((p[3] & 0xc0) != 0x80) return false;
		if(out) *out = ((p[0] & 0x07) << 18) + ((p[1] & 0x3f) << 12) +
			((p[2] & 0x3f) << 6) + (p[3] & 0x3f);
		in += 4;
		return true;
	}
	else if(p[0] < 0xfc)
	{
		// five bytes (26bits)
		if((p[1] & 0xc0) != 0x80) return false;
		if((p[2] & 0xc0) != 0x80) return false;
		if((p[3] & 0xc0) != 0x80) return false;
		if((p[4] & 0xc0) != 0x80) return false;
		if(out) *out = ((p[0] & 0x03) << 24) + ((p[1] & 0x3f) << 18) +
			((p[2] & 0x3f) << 12) + ((p[3] & 0x3f) << 6) + (p[4] & 0x3f);
		in += 5;
		return true;
	}
	else if(p[0] < 0xfe)
	{
		// six bytes (31bits)
		if((p[1] & 0xc0) != 0x80) return false;
		if((p[2] & 0xc0) != 0x80) return false;
		if((p[3] & 0xc0) != 0x80) return false;
		if((p[4] & 0xc0) != 0x80) return false;
		if((p[5] & 0xc0) != 0x80) return false;
		if(out) *out = ((p[0] & 0x01) << 30) + ((p[1] & 0x3f) << 24) +
			((p[2] & 0x3f) << 18) + ((p[3] & 0x3f) << 12) +
			((p[4] & 0x3f) << 6) + (p[5] & 0x3f);
		in += 6;
		return true;
	}
	return false;
}


bool frame_buffer_t::clip(int &fx, int &fy, int &x, int &y, int &w, int &h) const
{
	if(x < 0)
		fx += -x, w -= -x, x = 0;
	if(y < 0)
		fy += -y, h -= -h, y = 0;
	if(x + w >= get_width())
		w -= (x + w) - get_width();
	if(y + h >= get_height())
		h -= (y + h) - get_height();

	return w >= 0 && h >= 0;
}


void frame_buffer_t::draw_char(int x, int y, int level, int ch, const font_base_t & font)
{
	font.put(ch, level, x, y, *this);
}

void frame_buffer_t::draw_text(int x, int y, int level, const __FlashStringHelper *ifsh, const font_base_t & font)
{
	PGM_P p = reinterpret_cast<PGM_P>(ifsh);

	uint8_t c;
	while(0 != (c = pgm_read_byte(p++)))
	{
		uint8_t buf[8] = {0};
		int utf8_bytes = get_utf8_bytes(c);
		int i;
		buf[0] = c;
		for(i = 1; i < utf8_bytes; ++i)
		{
			buf[i] = pgm_read_byte(p++);
			if(buf[i] == 0) break;
		}
		const uint8_t *bp = &buf[0];
		uint32_t wc = 0;
		if(utf8tow(bp, &wc))
		{
			font_base_t::metrics_t met = font.get_metrics(wc);
			if(met.exist)
			{
				draw_char(wc, level, x, y, font);
				x += met.w;
			}
		}
		else
			return;
	}
}

void frame_buffer_t::draw_text(int x, int y, int level, const char *s, const font_base_t & font)
{
	const uint8_t *p = reinterpret_cast<const uint8_t *>(s);

	uint32_t c = 0;
	while(true)
	{
		if(!*p) return;

		if(utf8tow(p, &c))
		{
			font_base_t::metrics_t met = font.get_metrics(c);
			if(met.exist)
			{
				draw_char(x, y, level, c, font);
				x += met.w;
			}
		}
		else
			return;
	}

}

void frame_buffer_t::fill(int level)
{
	for(int yy = 0; yy < LED_MAX_LOGICAL_ROW; ++yy)
	{
		for(int xx = 0; xx < LED_MAX_LOGICAL_COL; ++xx)
		{
			buffer[yy][xx] = level;
		}
	}
}

void frame_buffer_t::fill(int x, int y, int w, int h, int level)
{
	for(int yy = y; yy < y + h; ++yy)
	{
		for(int xx = x; xx < x + w; ++xx)
		{
			buffer[yy][xx] = level;
		}
	}
}

void frame_buffer_flip()
{
	if(&current_frame_buffer == buffers + 0)
	{
		current_frame_buffer = buffers[1];
		bg_frame_buffer      = buffers[0];
	}
	else
	{
		current_frame_buffer = buffers[0];
		bg_frame_buffer      = buffers[1];
	}
}
