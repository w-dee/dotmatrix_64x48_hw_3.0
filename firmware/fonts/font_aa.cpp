#include <Arduino.h>
#include "glyph_t.h"
#include "frame_buffer.h"
#include "font.h"
#include "font_aa.h"


const glyph_t * font_aa_t::get_glyph(int32_t chr) const
{
	int count = pgm_read_dword(&glyph_header.num_glyphs);

	uint32_t s = 0;
	uint32_t e = count;
	const glyph_t * g = nullptr;

	do
	{
		uint32_t m = (s+e)/2;
		switch(e - s)
		{
		case 0:
			// nothing found
			return nullptr;

		case 1:
			// last one found
			g = glyph_header.array + m;
			if(pgm_read_dword(&g->code_point) == chr)
				return g; // found
			return nullptr; // not found

		default:
			// do binary search
			g = glyph_header.array + m;
			int cp = pgm_read_dword(&g->code_point);
			if(cp == chr)
				return g; // found
			if(cp < chr)
				s = m;
			else
				e = m;
		}
	} while(1);


}

int font_aa_t::get_height() const
{
	return pgm_read_dword(&glyph_header.nominal_height);
}

font_base_t::metrics_t font_aa_t::get_metrics(int32_t chr) const
{
	const glyph_t * g = get_glyph(chr);
	metrics_t r;
	if(g)
	{
		r.w = pgm_read_byte(&g->ascend_x);
		r.h = pgm_read_byte(&g->h);
		r.exist = true;
	}
	else
	{
		r.exist = false;
	}
	return r;
}

void font_aa_t::put(int32_t chr, int level, int x, int y, frame_buffer_t & fb) const
{
	const glyph_t * g = get_glyph(chr);
	if(!g) return;	

	int fx = 0, fy = 0;
	int w = pgm_read_byte(&g->w), h = pgm_read_byte(&g->h);
	int stride = w;

	// clip font bounding box
	if(!fb.clip(fx, fy, x, y, w, h)) return;

	// draw the pattern
	const unsigned char *p = g->bitmap;

	for(int yy = y; yy < h+y; ++yy, ++fy)
	{
		const unsigned char * line = p + fy * w;
		int fxx = fx;
		for(int xx = x; xx < w+x; ++xx, ++fxx)
		{
			int alpha = pgm_read_byte(line + fxx);
			if(alpha)
			{
				int v = fb.get_point(xx, yy);
				v = alpha;
//				v = (((level - v) * alpha) >> 8 ) + v; // TODO: exact calculation
				fb.set_point(xx, yy, v);
			}
		}
	}
}

#include "large_digits.inc"
font_aa_t font_large_digits(LARGE_DIGITS);
#include "bold_digits.inc"
font_aa_t font_bold_digits(BOLD_DIGITS);
#include "week_names.inc"
font_aa_t font_week_names(WEEK_NAMES);

