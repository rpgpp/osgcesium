#ifndef _OC_VOLUME_DBZ_H_
#define _OC_VOLUME_DBZ_H_

#include "Volume.h"
#include "Humidity.h"

namespace OC
{
	class CDBZ : public CVolume
	{
	public:
		CDBZ();
		~CDBZ();

		osg::Image* readImage(String filename, String var_name);
		osg::Image* readImage2(String filename, String var_name);
		void readDBZ2Cesium(String filename, String var_name);

	};
}



#endif
