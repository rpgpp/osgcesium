#include "ProjectTool.h"

#define WGS84_COORD "+proj=longlat +datum=WGS84 +no_defs"
#define XIAN80_COORD "+proj=tmerc +lat_0=0 +lon_0=114 +k=1 +x_0=500000 +y_0=0 +a=6378140 +b=6356755.288157528 +units=m +no_defs"
#define BEIJING54_COORD "+proj=tmerc +lat_0=0 +lon_0=114 +k=1 +x_0=500000 +y_0=0 +ellps=krass +towgs84=15.8,-154.4,-82.3,0,0,0,0 +units=m +no_defs"

namespace OC
{
	namespace Modeling
	{
		ProjectTool::ProjectTool(String prjcs)
			:mPrjcs(prjcs)
		{
			local_srs = osgEarth::SpatialReference::get(mPrjcs);
			wgs84_srs = osgEarth::SpatialReference::get("wgs84");
		}

		ProjectTool::~ProjectTool(void)
		{

		}

		String ProjectTool::getPrjcs()
		{
			return mPrjcs;
		}

		void ProjectTool::setPrjcs(String prjcs)
		{
			mPrjcs = prjcs;
			local_srs = osgEarth::SpatialReference::get(mPrjcs);
		}

		CVector3 ProjectTool::toWorld(CVector2 LonLat)
		{
			return toWorld(CVector3(LonLat.x, LonLat.y,0.0));
		}

		CVector3 ProjectTool::toWorld(CVector3 LonLatH)
		{
			osgEarth::GeoPoint wgs84_point(osgEarth::SpatialReference::get("wgs84"), LonLatH.x, LonLatH.y, LonLatH.z, osgEarth::ALTMODE_ABSOLUTE);
			osg::Vec3d world_point;
			wgs84_point.toWorld(world_point);
			return CVector3(world_point.x(), world_point.y(), world_point.z());
		}

		CVector3 ProjectTool::worldToLonLatH(CVector3 worldposition)
		{
			osgEarth::GeoPoint world_point;
			world_point.fromWorld(osgEarth::SpatialReference::get("wgs84"), Vector3ToVec3d(worldposition));
			return CVector3(world_point.x(), world_point.y(), world_point.z());
		}

		CVector3 ProjectTool::localTo4326(CVector3 localPoint)
		{
			osg::Vec3d offset = Vector3ToVec3d(localPoint);
			offset.z() = 0.0f;

			osgEarth::GeoPoint gpoint(local_srs, offset);
			osgEarth::GeoPoint wgs84_point;
			gpoint.transform(wgs84_srs, wgs84_point);

			double longitude = wgs84_point.x();
			double latitude = wgs84_point.y();
			double height = localPoint.z;

			return CVector3(longitude, latitude, height);
		}

		osg::Matrix ProjectTool::computeTransform(CVector2 LonLat)
		{
			return computeTransform(CVector3(LonLat.x, LonLat.y,0.0f));
		}

		osg::Matrix ProjectTool::computeTransform(CVector3 LonLatH)
		{
			osg::Matrix out_l2w;
			osgEarth::GeoPoint wgs84_point(osgEarth::SpatialReference::get("wgs84"), LonLatH.x, LonLatH.y, LonLatH.z, osgEarth::ALTMODE_ABSOLUTE);
			wgs84_point.createLocalToWorld(out_l2w);
			return out_l2w;
		}

		osg::BoundingBox ProjectTool::computeRegion(CVector3 LonLatH, double tile_w, double tile_h)
		{
			osg::BoundingBox box;

			double longitudeRad = osg::DegreesToRadians(LonLatH.x);
			double latitudeRad = osg::DegreesToRadians(LonLatH.y);

			box.xMin() = longitudeRad - (tile_w * 0.5f * 0.000000156785 / std::cos(latitudeRad));
			box.xMax() = longitudeRad + (tile_w * 0.5f * 0.000000156785 / std::cos(latitudeRad));
			box.yMin() = latitudeRad - (tile_h * 0.5f * 0.000000156785);
			box.yMax() = latitudeRad + (tile_h * 0.5f * 0.000000156785);
			box.zMin() = 0;
			box.zMax() = LonLatH.z > 0 ? LonLatH.z : 40;

			return box;
		}

		void ProjectTool::transNode2Global(PipeNode* pipeNode)
		{
			CVector3 globalPos = CVector3::ZERO;

			CVector3 flatPosition = pipeNode->position();

			CVector3 lonLatH = localTo4326(flatPosition);
			osg::Matrix matrix = computeTransform(lonLatH);
			osg::Vec3d wordlPosition = matrix.getTrans();

			pipeNode->position() = Vec3ToVector3(wordlPosition);

			CVector3 worldZ = pipeNode->position();
			worldZ.normalise();

			pipeNode->setWorldZVector(worldZ);

			PipePointList& list = pipeNode->getAroundPointList();
			PipePointList::iterator it = list.begin();
			for (;it != list.end();it++)
			{
				CVector3 flatPosition = it->Position;
				CVector3 lonLatH = localTo4326(flatPosition);
				osg::Matrix matrix = computeTransform(lonLatH);
				osg::Vec3d wordlPosition = matrix.getTrans();
				it->Position = Vec3ToVector3(wordlPosition);
			}

		}

		void ProjectTool::transLine2Global(PipeLine* pipeLine)
		{
			{
				Vector3List& points = pipeLine->getLinPoint();

				if (points.size() > 0)
				{
					CVector3 center = CVector3::ZERO;
					for (Vector3List::size_type i = 0; i < points.size(); i++)
					{
						CVector3 flatPosition = points[i];
						CVector3 lonLatH = localTo4326(flatPosition);
						center += lonLatH;
						osg::Matrix matrix = computeTransform(lonLatH);
						osg::Vec3d wordlPosition = matrix.getTrans();
						points[i] = Vec3ToVector3(wordlPosition);
					}

					center /= points.size();
					osg::Matrix matrix = computeTransform(center);
					CVector3 worldZ = Vec3ToVector3(matrix.getTrans());
					worldZ.normalise();
					pipeLine->setWorldZVector(worldZ);
				}
			}

			std::vector<Vector3List>& extraLines = pipeLine->getExtraLine();
			int num = (int)extraLines.size();
			for (int i = 0; i < num; i++)
			{
				Vector3List& points = extraLines[i];
				for (Vector3List::size_type i = 0; i < points.size(); i++)
				{
					CVector3 flatPosition = points[i];
					CVector3 lonLatH = localTo4326(flatPosition);
					osg::Matrix matrix = computeTransform(lonLatH);
					osg::Vec3d wordlPosition = matrix.getTrans();
					points[i] = Vec3ToVector3(wordlPosition);
				}
			}
		}
	}
}
