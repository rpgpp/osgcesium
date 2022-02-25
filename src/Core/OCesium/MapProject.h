#ifndef _OC_MapProject_H__
#define _OC_MapProject_H__

#include "CesiumDefine.h"

namespace OC
{
	class _CesiumExport MapProject : public osg::Referenced
	{
	public:
		MapProject(String prjcs);
		~MapProject(void);

		String getPrjcs();
		void setPrjcs(String prjcs);
		CVector2				localTo4326(CVector2 localPoint);
		CVector3				localTo4326(CVector3 localPoint);
		const osgEarth::SpatialReference* getProjectSRS() { return local_srs; }
		static CVector3			worldToLonLatH(CVector3 worldposition);
		static CVector3			toWorld(CVector3 LonLatHeight);
		static CVector3			toWorld(CVector2 LonLat);
		static osg::Matrix		computeTransform(CVector2 LonLat);
		static osg::Matrix		computeTransform(CVector3 LonLatH);
		static osg::BoundingBox	computeRegion(CVector3 LonLatH, double tile_w, double tile_h);
		static CVector2 gaussToLH(double X, double Y, bool isSix = true);
	private:
		String							mPrjcs;
		osgEarth::SpatialReference*		local_srs;
		osgEarth::SpatialReference*		wgs84_srs;
	};
}

#endif



