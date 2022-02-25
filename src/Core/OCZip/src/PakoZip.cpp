#include "PakoZip.h"
#include "zip.h"

namespace OC
{
    PakoZip::PakoZip()
    {}
    PakoZip::~PakoZip()
    {}

#define CHUNK 32768
	bool PakoZip::compress(std::ostream& fout, const std::string& src)
    {
		int ret, flush = Z_FINISH;
		unsigned have;
		z_stream strm;
		unsigned char out[CHUNK];

		int level = 8;
		int stategy = Z_DEFAULT_STRATEGY;

		/* allocate deflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		ret = deflateInit2(&strm, level, Z_DEFLATED,
			-15,//15+16, // +16 to use gzip encoding
			8, // default
			stategy);
		if (ret != Z_OK) return false;

		strm.avail_in = (uInt)src.size();
		strm.next_in = (Bytef*)(&(*src.begin()));

		/* run deflate() on input until output buffer not full, finish
		   compression if all of source has been read in */
		do
		{
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = deflate(&strm, flush);    /* no bad return value */

			if (ret == Z_STREAM_ERROR)
			{
				return false;
			}

			have = CHUNK - strm.avail_out;
			if (have > 0) fout.write((const char*)out, have);

			if (fout.fail())
			{
				(void)deflateEnd(&strm);
				return false;
			}
		} while (strm.avail_out == 0);

		/* clean up and return */
		(void)deflateEnd(&strm);
		return true;
    }
}