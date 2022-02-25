#pragma once

#include "Volume.h"
#include "CinRadHeader.h"

namespace OC
{
	typedef std::vector<osg::ref_ptr<osg::Image> > ImageList;
	class ICinRadReader
	{
	public:
		ICinRadReader(Type type)
			:_type(type)
		{
			if (_type == CA)
			{
				_radius = 800;
				_radiusDoupplerV = 1600;
			}
		}
		virtual ~ICinRadReader() {}
		virtual void read(DataStreamPtr dataStream) {}
		virtual ImageList generateImage(float clampMin, float clampMax,bool autoResize = true) = 0;
		int t() { return 2 * _radius; }
		int s() { return 2 * _radius; }

		int						_radius = 460;
		int						_radiusDoupplerV = 920;
		Type					_type;
		ImageList				mFloatImageList;
		std::vector<float>		pitchList;
	};
}