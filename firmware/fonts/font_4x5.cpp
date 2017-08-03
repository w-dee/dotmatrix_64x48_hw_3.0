#include <Arduino.h>
#include "font_4x5.h"
#include "frame_buffer.h"

//! variable width glyph type
struct glyph_vw_t
{
	uint8_t width;
	uint8_t bitmap[5];
};

static constexpr unsigned char operator "" _b (const char *p, size_t)
{
	return
		((p[0]!=' ') << 7) + 
		((p[1]!=' ') << 6) + 
		((p[2]!=' ') << 5) + 
		((p[3]!=' ') << 4) + 
		((p[4]!=' ') << 3) +
		((p[5]!=' ') << 2) +
		((p[6]!=' ') << 1) +
		((p[7]!=' ') << 0) ;
}

const PROGMEM glyph_vw_t font_4x5_data[]= {
{ // '0'
4, {
	" @@     "_b,
	"@  @    "_b,
	"@  @    "_b,
	"@  @    "_b,
	" @@     "_b,
	}
},
{ // '1'
4, {
	" @@     "_b,
	"  @     "_b,
	"  @     "_b,
	"  @     "_b,
	" @@@    "_b,
	}
},
{ // '2'
4, {
	" @@     "_b,
	"@  @    "_b,
	"  @     "_b,
	" @      "_b,
	"@@@@    "_b,
	}
},
{ // '3'
4, {
	"@@@     "_b,
	"   @    "_b,
	" @@     "_b,
	"   @    "_b,
	"@@@     "_b,
	}
},
{ // '4'
4, {
	" @@     "_b,
	"@ @     "_b,
	"@ @     "_b,
	"@@@@    "_b,
	"  @     "_b,
	}
},
{ // '5'
4, {
	"@@@@    "_b,
	"@       "_b,
	"@@@     "_b,
	"   @    "_b,
	"@@@     "_b,
	}
},
{ // '6'
4, {
	" @@@    "_b,
	"@       "_b,
	"@@@     "_b,
	"@  @    "_b,
	" @@     "_b,
	}
},
{ // '7'
4, {
	"@@@@    "_b,
	"   @    "_b,
	"  @     "_b,
	"  @     "_b,
	"  @     "_b,
	}
},
{ // '8'
4, {
	" @@     "_b,
	"@  @    "_b,
	" @@     "_b,
	"@  @    "_b,
	" @@     "_b,
	}
},
{ // '9'
4, {
	" @@     "_b,
	"@  @    "_b,
	" @@@    "_b,
	"   @    "_b,
	"@@@     "_b,
	}
},
{ // ' '  - 10
1, {
	"        "_b,
	"        "_b,
	"        "_b,
	"        "_b,
	"        "_b,
	}
},
{ // '.'  - 11
1, {
	"        "_b,
	"        "_b,
	"        "_b,
	"        "_b,
	"@       "_b,
	}
},
{ // ':' - 12
1, {
	"        "_b,
	"@       "_b,
	"        "_b,
	"@       "_b,
	"        "_b,
	}
},
{ // L'℃' - 13
4, {
	"@       "_b,
	"  @@    "_b,
	" @      "_b,
	" @      "_b,
	"  @@    "_b,
	}
},
{ // 'h' - 14
3, {
	"@       "_b,
	"@       "_b,
	"@@      "_b,
	"@ @     "_b,
	"@ @     "_b,
	}
},
{ // '%' - 15
4, {
	"        "_b,
	"@  @    "_b,
	"  @     "_b,
	" @      "_b,
	"@  @    "_b,
	}
},


}; 


static int chr_to_index(int32_t chr)
{
	switch(chr)
	{
	case '0' ... '9': return chr - '0';
	case ' ': return 10;
	case '.': return 11;
	case ':': return 12;
	case L'℃': return 13;
	case 'h': return 14;
	case '%': return 15;
	default:;
	}
	return -1;
}

font_base_t::metrics_t font_4x5_t::get_metrics(int32_t chr) const
{
	int idx = chr_to_index(chr);
	if(idx == -1) return metrics_t{0,0,false}; // not found
	return metrics_t{pgm_read_byte( & (font_4x5_data[idx].width) ) + 1, 6, true};
}

void font_4x5_t::put(int32_t chr, int level, int x, int y, frame_buffer_t & fb) const
{
	int fx = 0, fy = 0;

	// get glyph pointer
	int idx = chr_to_index(chr);
	if(idx == -1) return; // not found
	const glyph_vw_t *p = font_4x5_data + idx;

	// clip font bounding box
	int w = pgm_read_byte( & (p->width) );
	int h = 5;
	if(!fb.clip(fx, fy, x, y, w, h)) return;

	// pixel loop
	for(int yy = y; yy < h+y; ++yy, ++fy)
	{
		unsigned char line = pgm_read_byte(p->bitmap + fy);
//		Serial.printf("line %d: %02x\r\n", fy, line);
		int fxx = fx;
		for(int xx = x; xx < w+x; ++xx, ++fxx)
		{
			if(line & (1<<(7-fxx)))
			{
//				Serial.printf("%d %d %d %d \r\n", fxx, fy, xx, yy);
				fb.set_point(xx, yy, level);
			}
		}
	}
}

font_4x5_t font_4x5;

