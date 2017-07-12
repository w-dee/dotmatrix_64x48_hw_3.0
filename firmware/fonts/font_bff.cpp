#include <Arduino.h>
#include <unistd.h>

using namespace std;

#include "font_bff.h"
#include "frame_buffer.h"


bff_font_t font_bff;

bff_font_t::glyph_cache_t::ptr_t bff_font_t::glyph_cache_t::find_and_touch(int32_t code_point)
{
	for(auto && i : array)
	{
		if(i && i->glyph_info.code_point == code_point)
		{
			// found; bring it to the top
			std::swap(i, array[top_point]);
			return array[top_point];
		}
	}
	return ptr_t();
}


void bff_font_t::glyph_cache_t::insert(ptr_t glyph)
{
	array[top_point] = glyph;
	++ top_point;
	if(top_point == array.size()) top_point = 0;
}



bff_font_t::bff_font_t() :
	file(),
	chrm_size(0),
	chrm_offs(0),
	btmp_size(0),
	btmp_offs(0),
	nominal_height(0) 
{
}


void bff_font_t::begin(uint32_t start_addr)
{
	// open the file
	file.init(start_addr);

	// some sanity check
	uint8_t buf[8];
	if(8 != file.read(buf, 8))
	{
		Serial.println(F("Error read font file\r\n"));
		goto error;
	}

	if(memcmp_P(buf, PSTR("BFF\0\x02\0\0\0"), 8))
	{
		Serial.println(F("Font signature not found; Forgot uploading font file ??\r\n signature:"));
		for(auto c : buf) {
			Serial.printf("%02x ", c);
		}
		Serial.println(F(""));
		goto error;
	}

	// read nominal height
	uint32_t tmp32;
	if(sizeof(tmp32) !=
		file.read(reinterpret_cast<uint8_t*>(&tmp32), sizeof(tmp32))) // TODO: endianness
		goto error;
	nominal_height = tmp32;

	// read numfiles
	uint32_t num_files;
	if(sizeof(num_files) !=
		file.read(reinterpret_cast<uint8_t*>(&num_files), sizeof(num_files))) // TODO: endianness
		goto error;


	// read file index
	for(uint32_t i = 0; i < num_files; ++i)
	{
		struct dir_t
		{
			char name[4];
			uint32_t size;
			uint32_t offset;
			uint32_t reserved;
		} dir;

		if(sizeof(dir_t) !=
			file.read(reinterpret_cast<uint8_t*>(&dir), sizeof(dir))) // TODO: endianness
			goto error;

		if(!memcmp_P(dir.name, PSTR("CHRM"), 4))
		{
			chrm_offs = dir.offset;
			chrm_size = dir.size;
		}
		else if(!memcmp_P(dir.name, PSTR("BTMP"), 4))
		{
			btmp_offs = dir.offset;
			btmp_size = dir.size;
		}
	}

	if(chrm_size == 0 || btmp_size == 0)
		goto error; // mandatory directory missing

	// calc num_glyphs
	num_glyphs = chrm_size / (4*8); // chrm (char map)'s each record is 32byte

Serial.printf("Font nominal height: %d\r\n", nominal_height);
Serial.printf("Font num_glyphs    : %d\r\n", num_glyphs);

	return;
error:
Serial.printf("Error opening glyph\r\n");
	file.close();

}

bff_font_t::~bff_font_t()
{
	file.close();
}

bff_font_t::glyph_info_t bff_font_t::get_glyph_info_by_index(uint32_t index) const
{
	glyph_info_t info;
	info.code_point = 0;

	if(index >= num_glyphs) return info;
	file.seek(chrm_offs + index * (4*8));
	if(sizeof(info) !=
		file.read(reinterpret_cast<uint8_t*>(&info), sizeof(info)))
		info.code_point = 0;

	return info;	
}

bff_font_t::glyph_info_t bff_font_t::get_glyph_info(uint32_t codepoint) const
{
	// all glyphs in BFF are sorted by its codepoint;
	// so we can use binary search here.
	uint32_t s = 0;
	uint32_t e = num_glyphs;

	glyph_info_t info;

	do
	{
		uint32_t m = (s+e)/2;

		switch(e - s)
		{
		case 0:
			// nothing found
			info.code_point = 0; 
			return info;

		case 1:
			// last one found
			info = get_glyph_info_by_index(m);
			if(info.code_point == codepoint)
				return info; // found
			else
			{
				info.code_point = 0;
				return info; // not match
			}

		default:
			// do binary search
			info = get_glyph_info_by_index(m);
			if(info.code_point == codepoint)
				return info; // found
			if(info.code_point < codepoint)
				s = m;
			else
				e = m;
		}
	} while(1);

}

std::shared_ptr<bff_font_t::glyph_t> bff_font_t::get_glyph(uint32_t codepoint) const
{
	glyph_info_t info = get_glyph_info(codepoint);

	if(info.code_point == 0)
		return std::shared_ptr<glyph_t>(new glyph_t(codepoint)); // returns non-existent glyph
	std::shared_ptr<glyph_t> g(new glyph_t(info));

	// uncompress the bitmap data
	uint8_t * compressed_data = nullptr;
	uint32_t compression_method = (info.flags & FLAGS_COMPRESSION_METHOD_MASK);
	if(info.bb_w > 0 && info.bb_h > 0 && info.compressed_size > 0 &&
		info.bitmap_offset != 0 &&
		(
			compression_method == FLAGS_COMPRESSION_METHOD_ZERO_RUNLENGTH_DIFF ||
			compression_method == FLAGS_COMPRESSION_METHOD_ZERO_RUNLENGTH )
		
		)
	{

		compressed_data = new uint8_t [info.compressed_size] ;
		file.seek(btmp_offs + info.bitmap_offset);
		if(info.compressed_size !=
			file.read(compressed_data, info.compressed_size))
				goto error;

		// process run-length
		unsigned int bitmap_size = info.bb_w * info.bb_h;
		uint8_t * cd = compressed_data;
		uint8_t * cd_limit = compressed_data + info.compressed_size;
		uint8_t * ucd = g->bitmap;
		uint8_t * ucd_limit = ucd + bitmap_size;

//fprintf(stderr, "flags:%d comp_size:%d uncomp_size:%d\n", info.flags, info.compressed_size, bitmap_size);

		while(ucd < ucd_limit && cd < cd_limit)
		{
			uint8_t ccd = *(cd++);
			if(ccd & 0x80)
			{
				*(ucd++) = ccd; // literal
			}
			else
			{
				uint8_t count = ccd & 0x3f;
				uint8_t v = 0;
				if(ccd & 0x40)
				{
					// non-zero running
					if(cd >= cd_limit) break;
					v = *(cd ++);
				}
				// fill
				while(ucd < ucd_limit && count --)
					*(ucd++) = v;
			}
		}
//fprintf(stderr, "%p %p %p %p\n", cd, cd_limit, ucd, ucd_limit);

		ucd = g->bitmap;
		if(compression_method == FLAGS_COMPRESSION_METHOD_ZERO_RUNLENGTH_DIFF)
		{
			// process differentiation
			for(int i = info.bb_w; i < bitmap_size; ++i)
				ucd[i] += ucd[i - info.bb_w];

			uint8_t prev = 0;
			for(int i = 0; i< bitmap_size ; ++i)
			{
				uint8_t v = (ucd[i] + prev) & 0x7f;
				prev = v;
				v = (v << 1) + (v >> 6); // 7bit grayscale to 8bit grayscale
				ucd[i] =  v;
			}
		}
		else
		{
			for(int i = 0; i< bitmap_size ; ++i)
			{
				uint8_t v = ucd[i] & 0x7f;
				v = (v << 1) + (v >> 6); // 7bit grayscale to 8bit grayscale
				ucd[i] =  v;
			}
		}
	}


error:
	if(compressed_data) delete [] compressed_data;
	return g;
}

std::shared_ptr<bff_font_t::glyph_t> bff_font_t::get_glyph_with_caching(uint32_t codepoint) const
{
	// find in cache
	std::shared_ptr<glyph_t> ptr = glyph_cache.find_and_touch(codepoint);
	if(!ptr)
	{
		// not found in the cache; retrieve and insert to the cache
		ptr = get_glyph(codepoint);
		glyph_cache.insert(ptr);
	}

	return ptr;
}

font_base_t::metrics_t bff_font_t::get_metrics(int32_t chr) const
{
	std::shared_ptr<glyph_t> ptr = get_glyph_with_caching(chr);
	if(ptr->glyph_info.flags & FLAGS_NOT_EXIST)
	{
		// non exsitent glyph;
		return metrics_t{0, 0, false};
	}
	return metrics_t {ptr->glyph_info.ascend_x / 64, nominal_height, true};
}


void bff_font_t::put(int32_t chr, int level, int x, int y, frame_buffer_t & fb) const
{
	std::shared_ptr<glyph_t> ptr = get_glyph_with_caching(chr);
	if(ptr->glyph_info.flags & FLAGS_NOT_EXIST) return; // non existent

	// adjust bounding box
	unsigned int bb_w = ptr->glyph_info.bb_w;
	x += ptr->glyph_info.bb_x;
	y += ptr->glyph_info.bb_y;
	int fx = 0, fy = 0;
	int w = ptr->glyph_info.bb_w, h = ptr->glyph_info.bb_h;

	// clip font bounding box
	if(!fb.clip(fx, fy, x, y, w, h)) return;

	// draw the pattern
	const unsigned char *p = ptr->bitmap;

	for(int yy = y; yy < h+y; ++yy, ++fy)
	{
		const uint8_t * line = p + fy * bb_w;
//		Serial.printf("line %d: %02x\r\n", fy, line);
		int fxx = fx;
		for(int xx = x; xx < w+x; ++xx, ++fxx)
		{
			int alpha = line[fxx];
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


