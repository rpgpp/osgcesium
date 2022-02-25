#ifndef _PAKO_ZIP_H__
#define _PAKO_ZIP_H__

#include "ZipDefine.h"
#include <sstream>

namespace OC
{
    class ZEXPORT2 PakoZip
    {
    public:
        PakoZip();
        ~PakoZip();
        static bool compress(std::ostream& fout, const std::string& src);
    };
}
#endif




