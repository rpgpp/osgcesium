#ifndef _OCZIP_DEFINE_H__
#define _OCZIP_DEFINE_H__

#ifdef _WIN32
#ifdef OCZip_EXPORTS
#define ZEXPORT2  __declspec(dllexport)
#else
#define ZEXPORT2  __declspec(dllimport)
#endif
#else
#define ZEXPORT2
#endif

#endif // !_oczip_define_h__

