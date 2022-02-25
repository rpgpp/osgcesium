#include "BZip2.h"
#include "./bz2/bzlib.h"

namespace OC
{
	size_t BZip2::openBZ2(std::string filename, std::ostream& os)
	{
        size_t size = 0;
        BZFILE* bzfile = BZ2_bzopen(filename.c_str(), "r");
        if (bzfile)
        {
#define BUFF_SIZE 10240
            char buffer[BUFF_SIZE];
            int read = BZ2_bzread(bzfile, buffer, BUFF_SIZE);
            while (read > 0)
            {
                size += read;
                os.write(buffer, read);
                read = BZ2_bzread(bzfile, buffer, BUFF_SIZE);
            }
            BZ2_bzclose(bzfile);
        }
        return size;
	}

}