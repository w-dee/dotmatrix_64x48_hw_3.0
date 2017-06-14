#ifndef FONT_AA_H
#define FONT_AA_H

#include "glyph_t.h"

//!  antialiased glyph font class
class font_aa_t : public font_base_t
{
	const glyph_header_t * glyph_header;

	const glyph_t * get_glyph(int32_t chr) const; 

public:
	font_aa_t(const glyph_header_t *glyph_header_) : glyph_header(glyph_header_) {}

	virtual int get_height() const; // including space

	virtual metrics_t get_metrics(int32_t chr) const;

	virtual void put(int32_t chr, int level, int x, int y, frame_buffer_t & fb) const;
};


#endif

