#include "OCMain/osg.h"
#include "OCMain/osgearth.h"
#include "OCMain/Environment.h"
#include "OCMain/ConfigManager.h"

namespace OC
{
	class OConvMapCacher
	{
	public:
		OConvMapCacher();
		~OConvMapCacher();
		void requestCes(int x, int y, int z, int level);
		void requestTDT(String map, int x, int y, int z, int level);
	};
}