#ifndef FONT_5x5_H
#define FONT_5x5_H

#include "font.h"

//! 5x5 extra small font class
class font_5x5_t : public font_base_t
{
public:
	virtual int get_height() const { return 6; } // including space

	virtual metrics_t get_metrics(int32_t chr) const;

	virtual void put(int32_t chr, int level, int x, int y, frame_buffer_t & fb) const;
};

extern font_5x5_t font_5x5;

#endif

