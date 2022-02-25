#pragma once

#include "OCServerDefine.h"
#include "OCUtility/StringConverter.h"

namespace OC
{
	namespace Server
	{
		class OCRegister
		{
		public:
			static String getRegister(StringVector params);
		};
	}
}