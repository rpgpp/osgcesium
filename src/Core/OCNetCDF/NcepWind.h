#ifndef _OC_NcepWind_H__
#define _OC_NcepWind_H__

#include "OCVolume/VolumeObject.h"

namespace OC
{
	class NcepWind : public Volume::CVolumeObject
	{
	public:
		NcepWind();
		~NcepWind();
		virtual bool convert(String filename);

		bool		flipX = false;
		float		interval = 0.0;
		String		nameU;
		String		nameV;
		String		levelX;
		String		levelY;
		String		levelZ;
	private:
		void readGrib(String filename);
	};
}

#endif // !_OC_NcepWind_H__
