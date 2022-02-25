#ifndef _PROJECT_TOOL_CLASS_H__
#define _PROJECT_TOOL_CLASS_H__

#include "PipeNode.h"
#include "PipeLine.h"
#include "OCMain/osgearth.h"

namespace OC
{
	namespace Modeling
	{
		class _ModelExport ProjectTool : public osg::Referenced
		{
		public:
			ProjectTool(String prjcs);
			~ProjectTool(void);

			void transNode2Global(PipeNode* pipeNode);
			void transLine2Global(PipeLine* pipeLine);

			String getPrjcs();
			void setPrjcs(String prjcs);

			CVector3				localTo4326(CVector3 localPoint);
			const osgEarth::SpatialReference* getProjectSRS() { return local_srs; }
			static CVector3			worldToLonLatH(CVector3 worldposition);
			static CVector3			toWorld(CVector3 LonLatHeight);
			static CVector3			toWorld(CVector2 LonLat);
			static osg::Matrix		computeTransform(CVector2 LonLat);
			static osg::Matrix		computeTransform(CVector3 LonLatH);
			static osg::BoundingBox	computeRegion(CVector3 LonLatH, double tile_w, double tile_h);
		private:
			String							mPrjcs;
			osgEarth::SpatialReference*		local_srs;
			osgEarth::SpatialReference*		wgs84_srs;
		};
	}
}

#endif



