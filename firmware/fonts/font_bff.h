#ifndef BFF_FONT_H_
#define BFF_FONT_H_

#include "font.h"
#include <memory>
#include "FS.h"

// file access emulation via flash access
class flash_linear_file_t
{
	uint32_t start_addr;
	uint32_t current_offs;

public:
	flash_linear_file_t() : start_addr(0), current_offs(0) {}
	~flash_linear_file_t() {}

	void init(uint32_t start) { start_addr = start; current_offs = 0; }

	uint32_t read(uint8_t * dest, uint32_t size)
	{
		bool success =
			ESP.flashRead(start_addr + current_offs,
				reinterpret_cast<uint32_t *>(dest), size);
		if(success)
		{
			current_offs += size;
			return size;
		}
		return 0;
	}

	void seek(uint32_t ofs) { current_offs = ofs; }

	void close() {}
};


// bff font handler
class bff_font_t : public font_base_t
{
protected:
	mutable flash_linear_file_t file; //!< the file object

	uint32_t chrm_size;
	uint32_t chrm_offs;
	uint32_t btmp_size;
	uint32_t btmp_offs;
	uint32_t num_glyphs;
	int nominal_height; //!< nominal height in px

public:
	static constexpr uint16_t FLAGS_NOT_EXIST = 0x40;
	static constexpr uint16_t FLAGS_COMPRESSION_METHOD_MASK = 0x07;
	static constexpr uint16_t FLAGS_COMPRESSION_METHOD_ZERO_RUNLENGTH_DIFF = 0x00;
	static constexpr uint16_t FLAGS_COMPRESSION_METHOD_ZERO_RUNLENGTH = 0x01;

	static constexpr int CACHE_LIMIT = 30;
	
#pragma pack(push, 1)
	struct glyph_info_t
	{
		int32_t code_point;
		int32_t ascend_x;
		int32_t ascend_y;
		uint32_t bitmap_offset;
		uint32_t compressed_size;
		uint16_t flags;
		uint16_t reserved0;
		int16_t bb_x;
		int16_t bb_y;
		uint16_t bb_w;
		uint16_t bb_h;
	};
#pragma pack(pop)

	struct glyph_t
	{
		glyph_info_t glyph_info;
		uint8_t * bitmap;
		glyph_t(int32_t code_point) : glyph_info(),  bitmap(nullptr)
		{
			glyph_info.flags = FLAGS_NOT_EXIST;
		}
		glyph_t(const glyph_info_t & info) :
			glyph_info(info), bitmap(new uint8_t [info.bb_w * info.bb_h])
		{;}
		~glyph_t() { if(bitmap) delete [] bitmap; }


	private:
		glyph_t(const glyph_t &);
		glyph_t(const glyph_t &&);
	};

private:
	class glyph_cache_t
	{
		typedef std::shared_ptr<glyph_t> ptr_t;
		typedef std::array<ptr_t, CACHE_LIMIT> array_t; 

		array_t array;
		size_t top_point;
	public:
		glyph_cache_t() : array(), top_point(0) {}

		ptr_t find_and_touch(int32_t code_point);
		void insert(ptr_t glyph); //!< insert new item; the item should not be already in cache 
	};

	mutable glyph_cache_t glyph_cache;

public: // font_base_t methods
	virtual int get_height() const { return nominal_height; } //!< returns font's nominal height in px

	virtual metrics_t get_metrics(int32_t chr) const;

	virtual void put(int32_t chr, int level, int x, int y, frame_buffer_t & fb) const;

public:
	bff_font_t();
	~bff_font_t();
	void begin(uint32_t start_addr);

	glyph_info_t get_glyph_info_by_index(uint32_t index) const;
	glyph_info_t get_glyph_info(uint32_t codepoint) const;
	std::shared_ptr<glyph_t> get_glyph(uint32_t codepoint) const;
	std::shared_ptr<glyph_t> get_glyph_with_caching(uint32_t codepoint) const;
};

extern bff_font_t font_bff;

#endif

