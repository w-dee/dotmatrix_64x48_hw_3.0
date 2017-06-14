#ifndef GLYPH_T_H
#define GLYPH_T_H

struct glyph_t
{
	const /*PROGMEM*/ uint8_t * bitmap; //!< pointer to the bitmap
	uint32_t code_point; //!< code point
	unsigned char w; //!< width
	unsigned char h; //!< height
	char ascend_x; //!< ascending amount
};

struct glyph_header_t
{
	const /*PROGMEM*/ glyph_t * array; //!< pointer to the array of glyphs
	int num_glyphs; //!< number of glyphs contained in
	unsigned char nominal_height; //!< nominal height
};


#endif


