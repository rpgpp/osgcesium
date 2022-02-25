#include "MapProject.h"

#define WGS84_COORD "+proj=longlat +datum=WGS84 +no_defs"
#define XIAN80_COORD "+proj=tmerc +lat_0=0 +lon_0=114 +k=1 +x_0=500000 +y_0=0 +a=6378140 +b=6356755.288157528 +units=m +no_defs"
#define BEIJING54_COORD "+proj=tmerc +lat_0=0 +lon_0=114 +k=1 +x_0=500000 +y_0=0 +ellps=krass +towgs84=15.8,-154.4,-82.3,0,0,0,0 +units=m +no_defs"

namespace OC
{
	MapProject::MapProject(String prjcs)
		:mPrjcs(prjcs)
	{
		local_srs = osgEarth::SpatialReference::get(mPrjcs);
		wgs84_srs = osgEarth::SpatialReference::get("wgs84");
	}

	MapProject::~MapProject(void)
	{

	}

	String MapProject::getPrjcs()
	{
		return mPrjcs;
	}

	void MapProject::setPrjcs(String prjcs)
	{
		mPrjcs = prjcs;
		local_srs = osgEarth::SpatialReference::get(mPrjcs);
	}

	CVector3 MapProject::toWorld(CVector2 LonLat)
	{
		return toWorld(CVector3(LonLat.x, LonLat.y,0.0));
	}

	CVector3 MapProject::toWorld(CVector3 LonLatH)
	{
		osgEarth::GeoPoint wgs84_point(osgEarth::SpatialReference::get("wgs84"), LonLatH.x, LonLatH.y, LonLatH.z, osgEarth::ALTMODE_ABSOLUTE);
		osg::Vec3d world_point;
		wgs84_point.toWorld(world_point);
		return CVector3(world_point.x(), world_point.y(), world_point.z());
	}

	CVector3 MapProject::worldToLonLatH(CVector3 worldposition)
	{
		osgEarth::GeoPoint world_point;
		world_point.fromWorld(osgEarth::SpatialReference::get("wgs84"), Vector3ToVec3d(worldposition));
		return CVector3(world_point.x(), world_point.y(), world_point.z());
	}
#if 0
	CVector2 MapProject::gaussToLH(double X, double Y, bool isSix)
	{
		double x, y, L0 = 0, B, L;
		x = X;
		y = Y;
		//double p = 57.295779513;
		//double p = 3437.7467708;
		double p = 206264.80625;
		//double p = 12375888.374808;

		for (int i = 1; Y / i >= 10; i = i * 10)
		{
			y = Y - (int)(Y / i) * i - 500000;
		}
		y = 504053;

		int D = Y / 1000000;
		int K = 0;
		if (isSix)
		{
			L0 = D * 6 - 3;
		}
		else
		{
			L0 = D * 3;
		}

		//按6°带克氏椭球反算
		double bt = x / 6367558.4969 * p;
		double BT = x / 6367558.4969;
		double c3 = cos(BT) * cos(BT);
		double c4 = sin(BT) * cos(BT);
		double Bf = (bt + (50221746 + (293622 + (2350 + 22.0 * c3) * c3) * c3) * c4 * 1e-10 * p) / p;
		double c5 = pow(cos(Bf), 2);
		double c6 = sin(Bf) * cos(Bf);
		double Nf = 6399698.902 - (21562.267 - (108.973 - 0.612 * c5) * c5) * c5;
		double Z = y / (Nf * cos(Bf));
		double b2 = (0.5 + 0.003369 * c5) * c6;
		double b3 = 0.333333 - (0.166667 - 0.001123 * c5) * c5;
		double b4 = 0.25 + (0.16161 + 0.00562 * c5) * c5;
		double b5 = 0.2 - (0.1667 - 0.0088 * c5) * c5;
		double z2 = pow(Z, 2);
		//double t1 = (Bf * p - (1.0 - (b4 - 0.12 * z2) * z2) * z2 * b2 * p);
		//long t2 = ((long)t1) % 3600;


		B = (Bf * p - (1.0 - (b4 - 0.12 * z2) * z2) * z2 * b2 * p) / 3600.0;
		L = L0 + ((1 - (b3 - b5 * z2) * z2) * Z * p) / 3600.0;
		return CVector2(L, B);
	}
#else
	inline int calcL0(int X)
	{
		int D = X / 1000000;
		return D * 6 - 3;
	}

	CVector2 MapProject::gaussToLH(double X, double Y, bool isSix)
	{
		int L0 = calcL0(4324768.792);

		String a = "+proj=tmerc +lat_0=0 +lon_0=117 +k=1 +x_0=500000 +y_0=0 +ellps=krass +units=m +no_defs";
		String b = "+proj=tmerc +lat_0=0 +lon_0=114 +k=1 +x_0=500000 +y_0=0 +ellps=IAU76 +units=m +no_defs";
		osgEarth::SpatialReference* local_srs = osgEarth::SpatialReference::get(b);
		osgEarth::SpatialReference* wgs84_srs = osgEarth::SpatialReference::get("wgs84");
		osgEarth::GeoPoint local_point(local_srs, 520584.7182, 4324768.792);
		//osgEarth::GeoPoint local_point(local_srs, 504053, 4055948);
		osgEarth::GeoPoint wgs84_point;
		bool ret = local_point.transform(wgs84_srs, wgs84_point);
		return CVector2(wgs84_point.x(), wgs84_point.y());
	}
#endif
	CVector2 MapProject::localTo4326(CVector2 localPoint)
	{
		CVector3 v = localTo4326(CVector3(localPoint.x, localPoint.y,0.0));
		return CVector2(v.x, v.y);
	}

	CVector3 MapProject::localTo4326(CVector3 localPoint)
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

	osg::Matrix MapProject::computeTransform(CVector2 LonLat)
	{
		return computeTransform(CVector3(LonLat.x, LonLat.y,0.0f));
	}

	osg::Matrix MapProject::computeTransform(CVector3 LonLatH)
	{
		osg::Matrix out_l2w;
		osgEarth::GeoPoint wgs84_point(osgEarth::SpatialReference::get("wgs84"), LonLatH.x, LonLatH.y, LonLatH.z, osgEarth::ALTMODE_ABSOLUTE);
		wgs84_point.createLocalToWorld(out_l2w);
		return out_l2w;
	}

	osg::BoundingBox MapProject::computeRegion(CVector3 LonLatH, double tile_w, double tile_h)
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
}
