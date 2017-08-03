#ifndef FONT_4x5_H
#define FONT_4x5_H

#include "font.h"

//! 4x5 extra small font class
class font_4x5_t : public font_base_t
{
public:
	virtual int get_height() const { return 6; } // including space

	virtual metrics_t get_metrics(int32_t chr) const;

	virtual void put(int32_t chr, int level, int x, int y, frame_buffer_t & fb) const;
};

extern font_4x5_t font_4x5;

#endif

