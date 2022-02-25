#ifndef _OC_NCEP_H_
#define _OC_NCEP_H_

#include "Volume.h"

namespace OC
{
	class Ncep : public CVolume
	{
	public:
		Ncep();
		~Ncep();
		osg::Image* readImage(String filename, String var_name);
	};
}

#endif
