#ifndef _OC_VOLUME_UV_H_
#define _OC_VOLUME_UV_H_

#include "OCUtility/Vector3i.h"
#include "NcVolume.h"

namespace OC
{
	namespace Volume
	{
		class NcUV : public NcVolume
		{
		public:
			NcUV();
			~NcUV();
			String nameU;
			String nameV;
			String levelX;
			String levelY;
			String levelZ;
			bool	flipX = false;
			float interval = 0.0;
			inline CVector3i getVarDim(String name);
			inline bool isValid();
			inline int getLevel();
			virtual bool convert(std::string filename);
			int _ncid;
		};
	}
}

#endif

