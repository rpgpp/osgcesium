#ifndef _OC_REFLECTIVE_H_
#define _OC_REFLECTIVE_H_

#include "Volume.h"
#include "Humidity.h"

namespace OC
{
	class Reflective : public CVolume
	{
	public:
		Reflective();
		~Reflective();

		osg::Image* readImage(String filename, String var_name);

	};
}



#endif
