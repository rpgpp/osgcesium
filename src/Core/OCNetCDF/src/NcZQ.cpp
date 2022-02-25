#include "NcZQ.h"

namespace OC
{
	namespace Volume
	{
	NcZQ::NcZQ(String varname)
		:mVarname(varname)
	{
		mClampMin = 0.0;
		mClampMax = 20.0;
	}

	NcZQ::~NcZQ()
	{

	}

	bool NcZQ::convert(std::string filename)
	{
		int _ncid, _varid;
		NcCheck(nc_open(filename.c_str(), NC_NOWRITE, &_ncid));
		NcCheck(nc_inq_varid(_ncid, mVarname.c_str(), &_varid));

		int ndimsp = 0;
		nc_inq_varndims(_ncid, _varid, &ndimsp);

		nc_type type;
		nc_inq_vartype(_ncid, _varid, &type);

		mDimension = ndimsp;

		int dimids[NC_MAX_VAR_DIMS];
		nc_inq_vardimid(_ncid, _varid, &dimids[0]);

		char dname[128];
		ncdiminq(_ncid, dimids[0], dname, &mDimZ);
		ncdiminq(_ncid, dimids[1], dname, &mDimY);
		ncdiminq(_ncid, dimids[2], dname, &mDimX);

		long xySize = mDimY * mDimX;
		long xyzSize = mDimZ * xySize;

		heiValues.resize(mDimZ);
		lonValues.resize(xySize);
		latValues.resize(xySize);
		elvValues.resize(xySize);
		var1Values.resize(xyzSize);

		int _lonid, _latid, _topoid, _ztqid;

		nc_inq_varid(_ncid, "lon", &_lonid);
		nc_inq_varid(_ncid, "lat", &_latid);
		nc_inq_varid(_ncid, "topo", &_topoid);
		nc_inq_varid(_ncid, "ztq", &_ztqid);

		nc_get_var(_ncid, _lonid, &lonValues[0]);
		nc_get_var(_ncid, _latid, &latValues[0]);
		nc_get_var(_ncid, _ztqid, &heiValues[0]);
		nc_get_var(_ncid, _topoid, &elvValues[0]);
		nc_get_var(_ncid, _varid, &var1Values[0]);


		float interval = 200.0f;

		float maxlon = -180.0f;
		float minlon = 180.0;
		float maxlat = -90.0f;
		float minlat = 90.0;
		int maxtopo = -1e+6;
		int mintopo = 1e+6;

		float maxzqt = heiValues[mDimZ - 1];

		long offset2 = 0;
		for (long y = 0; y < mDimY; y++)
		{
			for (long x = 0; x < mDimX; x++)
			{
				float lon = lonValues[offset2];
				float lat = latValues[offset2];
				int topo = elvValues[offset2];
				maxlon = max(maxlon, lon);
				maxlat = max(maxlat, lat);
				maxtopo = max(maxtopo, topo);
				minlon = min(minlon, lon);
				minlat = min(minlat, lat);
				mintopo = min(mintopo, topo);
				offset2++;
			}
		}

		int imageS = mDimX;
		int imageT = mDimY;
		int imageR = (maxzqt + maxtopo) / interval + 1;

		CRectangle extent;
		extent.setExtents(CVector2(minlon, minlat), CVector2(maxlon, maxlat));

		CVector2 center = extent.getCenter();

		float w_bottom = lonValues[mDimX - 1] - lonValues[0];
		float w_top = lonValues[xySize - 1] - lonValues[xySize - mDimX];

		float h = extent.getHeight();
		float w_meters = width_in_meter(maxlat, w_bottom);
		float h_meters = height_in_meter(h);
		float d_meters = maxtopo + heiValues[mDimZ - 1];

		CVector3 scale;
		scale.x = w_meters / imageS;
		scale.y = h_meters / imageT;
		scale.z = d_meters / imageR;

		osg::ref_ptr<osg::Image> image = _CreateVolumeImage(imageS, imageT, imageR);

		long offset3 = 0;
		for (long z = 0; z < mDimZ; z++)
		{
			long offset2 = 0;
			for (long y = 0; y < imageT; y++)
			{
				for (long x = 0; x < imageS; x++)
				{
					float v = var1Values[offset3];
					PRC1(v);

					osg::Vec4ub color = _EncodeValue(v);
					unsigned char* ptr = (unsigned char*)image->data(x, y, z);

					if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] == 0 && ptr[3] == 0)
					{
						if (z > 0)
						{
							unsigned char* data2 = (unsigned char*)image->data(x, y, z - 1);
							ptr[0] = data2[0];
							ptr[1] = data2[1];
							ptr[2] = data2[2];
							ptr[3] = data2[3];
						}
						else
						{
							ptr[0] = color.r();
							ptr[1] = color.g();
							ptr[2] = color.b();
							ptr[3] = color.a();
						}
					}

					osg::Vec2 normal2D(0.0, 0.0);
					if (y > 0 && x > 0 && y < imageT - 1 && x < imageS - 1)
					{
						float px = var1Values[offset3 + 1];
						float nx = var1Values[offset3 - 1];
						float py = var1Values[offset3 + imageS];
						float ny = var1Values[offset3 - imageS];
						normal2D = computeNormal(v, px, nx, px, ny);
					}

					int r = (heiValues[z] + elvValues[offset2]) / interval;
					ptr = (unsigned char*)image->data(x, y, r);
					ptr[0] = color.r();
					ptr[1] = color.g();
					ptr[2] = normal2D.x();
					ptr[3] = normal2D.y();
					offset2++;
					offset3++;
				}
			}
		}


		setPosition(center);
		setMeter(osg::Vec3d(w_meters, h_meters, d_meters));
		setUint8Image(image);

		return true;
	}
}
}
