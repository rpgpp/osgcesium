#ifndef _OC_HTTP_SERVER_DEFINE_H_
#define _OC_HTTP_SERVER_DEFINE_H_

#if defined(_MSC_VER)
#ifndef OCHttpServer_EXPORTS
#define _HttpServerExport  __declspec(dllimport)
#else
#define _HttpServerExport  __declspec(dllexport)
#endif
#else
#define _HttpServerExport
#endif

#include "OCMain/stl.h"
#define OCLogStream Singleton(OC::LogManager).stream()

#endif