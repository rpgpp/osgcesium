#ifndef _OC_HTTP_Client_DEFINE_H_
#define _OC_HTTP_Client_DEFINE_H_

#if defined(_MSC_VER)
#ifndef OCHttpClient_EXPORTS
#define _HttpClientExport  __declspec(dllimport)
#else
#define _HttpClientExport  __declspec(dllexport)
#endif
#else
#define _HttpClientExport
#endif

#include "OCMain/stl.h"

#endif