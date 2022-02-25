#include "DBZ.h"

namespace OC
{
	CDBZ::CDBZ()
	{

	}

	CDBZ::~CDBZ()
	{

	}
#define VEqual(a,b) osg::equivalent(a,b,1e-3f)
#define CORRECT(v) if(Math::isNaN(v) || VEqual(v, missing_value) || VEqual(v, fill_value))v=0.0
	osg::Image* CDBZ::readImage(String filename, String var_name)
	{
		float fill_value = -31.328f;
		float missing_value = -31.328f;
		float _OriginX = 0.0;
		float _OriginY = 0.0;
		float unit = 1000.0f;

		int _varid;
		NCFunction(nc_inq_varid(_ncid, var_name.c_str(), &_varid));

		int ndimsp, nvarsp, nattsp, unlimdimp;
		ncinquire(_ncid, &ndimsp, &nvarsp, &nattsp, &unlimdimp);

		for (int i=0;i<nattsp;i++)
		{
			char attname[128];
			ncattname(_ncid, NC_GLOBAL, i, attname);
			nc_type xtypep;
			int lenp;
			ncattinq(_ncid, NC_GLOBAL, attname, &xtypep, &lenp);
			if (String(attname) == "_FillValue" && xtypep == NC_FLOAT)
			{
				ncattget(_ncid, NC_GLOBAL, attname, &fill_value);
			}
			else if (String(attname) == "OriginX" && xtypep == NC_FLOAT)
			{
				ncattget(_ncid, NC_GLOBAL, attname, &_OriginX);
			}
			else if (String(attname) == "OriginY" && xtypep == NC_FLOAT)
			{
				ncattget(_ncid, NC_GLOBAL, attname, &_OriginY);
			}
		}


		nc_type type;
		nc_inq_vartype(_ncid, _varid, &type);

		String dir, basename, ext;
		StringUtil::splitFullFilename(filename, basename, ext, dir);

		NC_ZQ zq;
		zq._Dimension = ndimsp;

		int dimids[NC_MAX_VAR_DIMS];
		nc_inq_vardimid(_ncid, _varid, &dimids[0]);

		char dname[128];
		if (ndimsp > 3)
		{
			ncdiminq(_ncid, dimids[1], dname, &zq._dimz);
			ncdiminq(_ncid, dimids[2], dname, &zq._dimy);
			ncdiminq(_ncid, dimids[3], dname, &zq._dimx);
		}
		else
		{
			ncdiminq(_ncid, dimids[0], dname, &zq._dimz);
			ncdiminq(_ncid, dimids[1], dname, &zq._dimy);
			ncdiminq(_ncid, dimids[2], dname, &zq._dimx);
		}


		long xySize = zq._dimy * zq._dimx;
		long xyzSize = zq._dimz * zq._dimy * zq._dimx;

		zq.ztq = new float[zq._dimz];
		zq.lon = new float[zq._dimx];
		zq.lat = new float[zq._dimy];
		zq.zq = new float[xyzSize];

		int _lonid, _latid, _ztqid;

		nc_inq_varid(_ncid, "X", &_lonid);
		nc_inq_varid(_ncid, "Y", &_latid);
		nc_inq_varid(_ncid, "height", &_ztqid);

		nc_get_var(_ncid, _lonid, zq.lon);
		nc_get_var(_ncid, _latid, zq.lat);
		nc_get_var(_ncid, _ztqid, zq.ztq);
		nc_get_var(_ncid, _varid, zq.zq);


		float interval = 1000.0f;

		float minlon = _OriginX;
		float minlat = _OriginY;

		float w_meters = zq._dimx * unit;
		float h_meters = zq._dimy * unit;

        float h = height_in_degree(h_meters);
        float w = width_in_degree(minlat - 0.5 * h, w_meters);//用底的纬度来算，所以减去一半高度

		float maxzqt = zq.ztq[zq._dimz - 1];

		int mutisample = Singleton(ConfigManager).getIntValue("MultiSampleDBZ",1);

		int imageS = zq._dimx;
		int imageT = zq._dimy;
		int imageR = zq._dimz * mutisample;

		CVector2 center(minlon, minlat);

		float d_meters = maxzqt * unit;

		CVector3 scale;
		scale.x = w_meters / imageS;
		scale.y = h_meters / imageT;
		scale.z = d_meters / imageR;

		osg::ref_ptr<osg::Image> image = new osg::Image;
		image->allocateImage(imageS, imageT, imageR, GL_RGBA, GL_UNSIGNED_BYTE);
		int component = osg::Image::computeNumComponents(image->getPixelFormat());
		memset(image->data(0, 0, 0), 0, imageS * imageT * imageR * component);

        float clampMin = 0.0;
        float clampMax = 70.0;
        float scaler = clampMax - clampMin;

		long validCount = 0;
		long offset = 0;
		for (long z = 0; z < imageR; z++)
		{
			for (long y = 0; y < imageT; y++)
			{
				for (long x = 0; x < imageS; x++, offset++)
				{
					long idx = offset;
					float v = zq.zq[idx];
					osg::Vec4 color(0.0,0.0,0.0,0.0);

					osg::Vec2 normal2D(0.0, 0.0);
					if (!Math::isNaN(v) && !VEqual(v, missing_value) && !VEqual(v, fill_value))
					{
						color = RGBAEncoder::encodeFloatRGBA(Math::Clamp((v - clampMin) / scaler, 0.0f, 0.999999f)) * 255.0;
						validCount++;
						if (y > 0 && x > 0 && y < imageT - 1 && x < imageS - 1)
						{
							float px = v + (zq.zq[idx + 1] - v) / imageS;
							float nx = v - (v - zq.zq[idx - 1]) / imageS;
							float py = v + (zq.zq[idx + imageS] - v) / imageT;
							float ny = v - (v - zq.zq[idx - imageS]) / imageT;
							CORRECT(px);
							CORRECT(nx);
							CORRECT(py);
							CORRECT(ny);
							if (px < clampMin) px = clampMin;
							if (nx < clampMin) nx = clampMin;
							if (py < clampMin) py = clampMin;
							if (ny < clampMin) ny = clampMin;
							float dx = px - nx;
							float dy = py - ny;
							if (dx != 0.0 || dy != 0.0)
							{
								normal2D = osg::Vec2(dx, dy);
								normal2D.normalize();
								normal2D *= 0.5;
								PRC1(normal2D.x());
								PRC1(normal2D.y());
								normal2D += osg::Vec2(0.5, 0.5);
							}
						}
					}

					color[2] = normal2D.x() * 255.0;
					color[3] = normal2D.y() * 255.0;

					unsigned char* data = (unsigned char*)image->data(x, y, z);
					data[0] = round(color.r());
					data[1] = round(color.g());
					data[2] = round(color.b());
					data[3] = round(color.a());
				}
			}
		}

		std::cout << "validCount:" << validCount << endl;

		setUserData(image, "scale", scale);
		setUserData(image, "position", CVector3(center.x, center.y, 0));
        setUserData(image, "meter", CVector3(w_meters, h_meters, d_meters));
        setUserData(image, "u_s_o", CVector3(scaler, clampMin, clampMax));
		return image.release();
	}

	osg::Image* CDBZ::readImage2(String filename, String var_name)
	{
		float fill_value = -31.328f;
		float missing_value = -31.328f;
		float _OriginX = 0.0;
		float _OriginY = 0.0;
		float unit = 1000.0f;

		int _varid;
		NCFunction(nc_inq_varid(_ncid, var_name.c_str(), &_varid));

		int ndimsp, nvarsp, nattsp, unlimdimp;
		ncinquire(_ncid, &ndimsp, &nvarsp, &nattsp, &unlimdimp);

		for (int i = 0; i < nattsp; i++)
		{
			char attname[128];
			ncattname(_ncid, NC_GLOBAL, i, attname);
			nc_type xtypep;
			int lenp;
			ncattinq(_ncid, NC_GLOBAL, attname, &xtypep, &lenp);
			if (String(attname) == "_FillValue" && xtypep == NC_FLOAT)
			{
				ncattget(_ncid, NC_GLOBAL, attname, &fill_value);
			}
			else if (String(attname) == "OriginX" && xtypep == NC_FLOAT)
			{
				ncattget(_ncid, NC_GLOBAL, attname, &_OriginX);
			}
			else if (String(attname) == "OriginY" && xtypep == NC_FLOAT)
			{
				ncattget(_ncid, NC_GLOBAL, attname, &_OriginY);
			}
		}


		nc_type type;
		nc_inq_vartype(_ncid, _varid, &type);

		String dir, basename, ext;
		StringUtil::splitFullFilename(filename, basename, ext, dir);

		NC_ZQ zq;
		zq._Dimension = ndimsp;

		int dimids[NC_MAX_VAR_DIMS];
		nc_inq_vardimid(_ncid, _varid, &dimids[0]);

		char dname[128];
		if (ndimsp > 3)
		{
			ncdiminq(_ncid, dimids[1], dname, &zq._dimz);
			ncdiminq(_ncid, dimids[2], dname, &zq._dimy);
			ncdiminq(_ncid, dimids[3], dname, &zq._dimx);
		}
		else
		{
			ncdiminq(_ncid, dimids[0], dname, &zq._dimz);
			ncdiminq(_ncid, dimids[1], dname, &zq._dimy);
			ncdiminq(_ncid, dimids[2], dname, &zq._dimx);
		}


		long xySize = zq._dimy * zq._dimx;
		long xyzSize = zq._dimz * zq._dimy * zq._dimx;

		zq.topo = new int[zq._dimz];
		zq.lon = new float[zq._dimx];
		zq.lat = new float[zq._dimy];
		zq.zq = new float[xyzSize];

		int _lonid, _latid, _ztqid;

		nc_inq_varid(_ncid, "X", &_lonid);
		nc_inq_varid(_ncid, "Y", &_latid);
		nc_inq_varid(_ncid, "height", &_ztqid);

		nc_get_var(_ncid, _lonid, zq.lon);
		nc_get_var(_ncid, _latid, zq.lat);
		nc_get_var(_ncid, _ztqid, zq.topo);
		nc_get_var(_ncid, _varid, zq.zq);


		float interval = 1000.0f;

		float minlon = _OriginX;
		float minlat = _OriginY;

		float w_meters = zq._dimx * unit;
		float h_meters = zq._dimy * unit;

		float h = height_in_degree(h_meters);
		float w = width_in_degree(minlat - 0.5 * h, w_meters);//用底的纬度来算，所以减去一半高度

		float maxzqt = zq.topo[zq._dimz - 1];

		int mutisample = Singleton(ConfigManager).getIntValue("MultiSampleDBZ", 1);

		int imageS = zq._dimx;
		int imageT = zq._dimy;
		int imageR = zq._dimz * mutisample;

		CVector2 center(minlon, minlat);

		float d_meters = maxzqt;

		CVector3 scale;
		scale.x = w_meters / imageS;
		scale.y = h_meters / imageT;
		scale.z = d_meters / imageR;

		osg::ref_ptr<osg::Image> image = new osg::Image;
		image->allocateImage(imageS, imageT, imageR, GL_RGBA, GL_UNSIGNED_BYTE);
		int component = osg::Image::computeNumComponents(image->getPixelFormat());
		memset(image->data(0, 0, 0), 0, imageS * imageT * imageR * component);

		float clampMin = 0.0;
		float clampMax = 70.0;
		float scaler = clampMax - clampMin;

		long validCount = 0;
		long offset = 0;
		for (long z = 0; z < imageR; z++)
		{
			for (long y = 0; y < imageT; y++)
			{
				for (long x = 0; x < imageS; x++, offset++)
				{
					long idx = offset;
					float v = zq.zq[idx];
					osg::Vec4 color(0.0, 0.0, 0.0, 0.0);

					osg::Vec2 normal2D(0.0, 0.0);
					if (!Math::isNaN(v) && !VEqual(v, missing_value) && !VEqual(v, fill_value))
					{
						color = RGBAEncoder::encodeFloatRGBA(Math::Clamp((v - clampMin) / scaler, 0.0f, 0.999999f)) * 255.0;
						validCount++;
						if (y > 0 && x > 0 && y < imageT - 1 && x < imageS - 1)
						{
							float px = v + (zq.zq[idx + 1] - v) / imageS;
							float nx = v - (v - zq.zq[idx - 1]) / imageS;
							float py = v + (zq.zq[idx + imageS] - v) / imageT;
							float ny = v - (v - zq.zq[idx - imageS]) / imageT;
							CORRECT(px);
							CORRECT(nx);
							CORRECT(py);
							CORRECT(ny);
							if (px < clampMin) px = clampMin;
							if (nx < clampMin) nx = clampMin;
							if (py < clampMin) py = clampMin;
							if (ny < clampMin) ny = clampMin;
							float dx = px - nx;
							float dy = py - ny;
							if (dx != 0.0 || dy != 0.0)
							{
								normal2D = osg::Vec2(dx, dy);
								normal2D.normalize();
								normal2D *= 0.5;
								PRC1(normal2D.x());
								PRC1(normal2D.y());
								normal2D += osg::Vec2(0.5, 0.5);
							}
						}
					}

					color[2] = normal2D.x() * 255.0;
					color[3] = normal2D.y() * 255.0;

					unsigned char* data = (unsigned char*)image->data(x, y, z);
					data[0] = color.r();
					data[1] = color.g();
					data[2] = color.b();
					data[3] = color.a();
				}
			}
		}

		std::cout << "validCount:" << validCount << endl;

		setUserData(image, "position", CVector3(center.x, center.y, 0));
		setUserData(image, "meter", CVector3(w_meters, h_meters, d_meters));
		setUserData(image, "u_s_o", CVector3(scaler, clampMin, clampMax));

		return image.release();
	}

	void CDBZ::readDBZ2Cesium(String filename, String var_name)
	{
		//int _varid;
		//nc_inq_varid(_ncid, var_name.c_str(), &_varid);

		//int ndimsp = 0;
		//nc_inq_varndims(_ncid, _varid, &ndimsp);

		//nc_type type;
		//nc_inq_vartype(_ncid, _varid, &type);

		//String dir, basename, ext;
		//StringUtil::splitFullFilename(filename, basename, ext, dir);
		//mTransferFunction = new CFileTransferFunction;
		//mTransferFunction->read(dir + var_name + ".tf");

		//NC_ZQ zq;
		//zq._Dimension = ndimsp;

		//int dimids[NC_MAX_VAR_DIMS];
		//nc_inq_vardimid(_ncid, _varid, &dimids[0]);

		//char dname[128];
		//ncdiminq(_ncid, dimids[1], dname, &zq._dimz);
		//ncdiminq(_ncid, dimids[2], dname, &zq._dimy);
		//ncdiminq(_ncid, dimids[3], dname, &zq._dimx);

		//long xySize = zq._dimy * zq._dimx;
		//long xyzSize = zq._dimz * zq._dimy * zq._dimx;

		//zq.ztq = new float[zq._dimz];
		//zq.lon = new float[zq._dimx];
		//zq.lat = new float[zq._dimy];
		//zq.zq = new float[xyzSize];

		//int _lonid, _latid, _ztqid;

		//nc_inq_varid(_ncid, "X", &_lonid);
		//nc_inq_varid(_ncid, "Y", &_latid);
		//nc_inq_varid(_ncid, "height", &_ztqid);

		//nc_get_var(_ncid, _lonid, zq.lon);
		//nc_get_var(_ncid, _latid, zq.lat);
		//nc_get_var(_ncid, _ztqid, zq.ztq);
		//nc_get_var(_ncid, _varid, zq.zq);


		//float interval = 1000.0f;

		//float maxlon = -180.0f;
		//float minlon = 180.0;
		//float maxlat = -90.0f;
		//float minlat = 90.0;

		//float maxzqt = zq.ztq[zq._dimz - 1];

		//for (long y = 0; y < zq._dimy; y++)
		//{
		//	float lat = zq.lat[y];
		//	maxlat = max(maxlat, lat);
		//	minlat = min(minlat, lat);
		//}

		//for (long x = 0; x < zq._dimx; x++)
		//{
		//	float lon = zq.lon[x];
		//	maxlon = max(maxlon, lon);
		//	minlon = min(minlon, lon);
		//}

		//int imageS = zq._dimx;
		//int imageT = zq._dimy;
		//int imageR = maxzqt / interval + 1;

		//CRectangle extent;
		//extent.setExtents(CVector2(minlon, minlat), CVector2(maxlon, maxlat));

		//CVector2 center = extent.getCenter();

		//float w = extent.getWidth();
		//float h = extent.getHeight();

		//float w_meters = width_in_meter(maxlat, w);
		//float h_meters = height_in_meter(h);

		//osg::ref_ptr<osg::Image> image = new osg::Image;
		//image->allocateImage(imageS, imageT, imageR, GL_RGBA, GL_UNSIGNED_BYTE);

		//memset(image->data(0, 0, 0), 0, imageS * imageT * imageR * 4);

		////
		//ProjectTool projectTool("");
		//osg::Matrix matrix = projectTool.computeTransform(center);
		//osg::Vec3d RC_Center = matrix.getTrans();
		//auto computeOffset = [&projectTool, &RC_Center](CVector3 LonLatH)
		//{
		//	osg::Matrix matrix = projectTool.computeTransform(LonLatH);
		//	return matrix.getTrans() - RC_Center;
		//};

		//osg::ref_ptr<Cesium::InstanceInfo> instanceinfo = new Cesium::InstanceInfo(1004590);

		////PointCloud

		//long validCount = 0;
		//long offset3 = 0;
		//for (long z = 0; z < zq._dimz; z++)
		//{
		//	for (long y = 0; y < imageT; y++)
		//	{
		//		for (long x = 0; x < imageS; x++)
		//		{
		//			float v = zq.zq[offset3++];
		//			if (Math::isNaN(v))
		//			{
		//				continue;
		//			}

		//			osg::Vec4 color = getColor(v);

		//			//
		//			osg::Vec3d offset = computeOffset(CVector3(zq.lon[x], zq.lat[y], zq.ztq[z]));
		//			instanceinfo->setPosition(validCount, offset);
		//			instanceinfo->setColor(validCount, color / 255.0);
		//			validCount++;
		//			//

		//			unsigned char* data = (unsigned char*)image->data(x, y, z);

		//			data[0] = color.r();
		//			data[1] = color.g();
		//			data[2] = color.b();
		//			data[3] = 64;

		//			if (z > 0)
		//			{
		//				data = (unsigned char*)image->data(x, y, z - 1);
		//				if (data[3] == 64)
		//				{
		//					data[3] = 128;
		//				}
		//			}
		//		}
		//	}
		//}

		//{
		//	String pntsFile = StringUtil::replaceAll(filename, ".nc", ".pnts");
		//	String out_tileset = StringUtil::replaceAll(filename, ".nc", ".json");

		//	std::string featureTableJSON, featureTableBody;
		//	instanceinfo->RTC_CENTER = matrix.getTrans();
		//	instanceinfo->RTC_CENTER = osg::Vec3();
		//	instanceinfo->getPntsTableJSON(featureTableJSON, featureTableBody);
		//	Cesium::X3DMWriter::writePNTS(pntsFile, featureTableJSON, featureTableBody, "", "");

		//	{
		//		osg::ref_ptr<TDTILES::Tileset> tileset = new TDTILES::Tileset;
		//		tileset->asset()->version() = "1.0";
		//		tileset->asset()->tilesetVersion() = "1.0.0-OC23dtiles";

		//		tileset->root() = new TDTILES::Tile;
		//		tileset->root()->transform() = osg::Matrix::translate(matrix.getTrans());

		//		double w = w_meters;
		//		double h = h_meters;

		//		tileset->geometricError() = max(w, h) / 18.0;

		//		osg::BoundingBox box = projectTool.computeRegion(CVector3(center.x, center.y, 0.0), w, h);
		//		box.zMin() = 0;
		//		box.zMax() = 200;

		//		tileset->root()->geometricError() = 0;
		//		tileset->root()->refine() = TDTILES::REFINE_ADD;
		//		tileset->root()->boundingVolume()->region() = box;
		//		tileset->root()->content()->uri() = osgDB::getSimpleFileName(pntsFile);

		//		std::ofstream fout(out_tileset.c_str());
		//		Util::Json::Value json = tileset->getJSON();
		//		Util::Json::StyledStreamWriter writer;
		//		writer.write(fout, json);
		//		fout.close();
		//	}
		//}
	}

}