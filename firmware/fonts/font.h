#ifndef FONT_H
#define FONT_H

class frame_buffer_t;

//! abstract simple font class
class font_base_t
{
public:
	struct metrics_t
	{
		int w;
		int h;
		bool exist;
	};

	virtual int get_height() const = 0; //!< returns font's nominal height in px

	virtual metrics_t get_metrics(int32_t chr) const = 0; //!< returns font metrics of given character code

	virtual void put(int32_t chr, int level, int x, int y, frame_buffer_t & fb) const = 0;
		//!< put a character to given framebuffer
};

#endif

