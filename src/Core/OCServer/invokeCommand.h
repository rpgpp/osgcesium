#ifndef _INVOKE_COMMAND_H__
#define _INVOKE_COMMAND_H__

#include "OCServer.h"
#include "OCMain/JsonPackage.h"

namespace OC
{
	namespace Server
	{
		String invoke(OC::String jsonStr);
	}
}

#endif // !_INVOKE_COMMAND_H__

