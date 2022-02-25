#include "NcDBZ.h"

#define CORRECT(v) if(Math::isNaN(v) || Math::RealEqual(v, missing_value) || Math::RealEqual(v, fill_value))v=mClampMin

namespace OC
{namespace Volume {

	NcDBZ::NcDBZ(String varname)
		:mVarname(varname)
	{
		mClampMin = 0.0;
		mClampMax = 70.0;
	}

	NcDBZ::~NcDBZ()
	{

	}

	bool NcDBZ::convert(std::string filename)
	{
		int _ncid,_varid;
		NcCheck(nc_open(filename.c_str(), NC_NOWRITE, &_ncid));
		NcCheck(nc_inq_varid(_ncid, mVarname.c_str(), &_varid));

		int ndimsp, nvarsp, nattsp, unlimdimp;
		ncinquire(_ncid, &ndimsp, &nvarsp, &nattsp, &unlimdimp);

		float _OriginX = 0.0;
		float _OriginY = 0.0;
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

		mDimension = ndimsp;

		int dimids[NC_MAX_VAR_DIMS];
		nc_inq_vardimid(_ncid, _varid, &dimids[0]);

		char dname[128];
		if (ndimsp > 3)
		{
			ncdiminq(_ncid, dimids[1], dname, &mDimZ);
			ncdiminq(_ncid, dimids[2], dname, &mDimY);
			ncdiminq(_ncid, dimids[3], dname, &mDimX);
		}
		else
		{
			ncdiminq(_ncid, dimids[0], dname, &mDimZ);
			ncdiminq(_ncid, dimids[1], dname, &mDimY);
			ncdiminq(_ncid, dimids[2], dname, &mDimX);
		}

		long xySize = mDimY * mDimX;
		long xyzSize = mDimZ * mDimY * mDimX;

		heiValues.resize(mDimZ);
		lonValues.resize(mDimX);
		latValues.resize(mDimY);
		var1Values.resize(xyzSize);

		int _lonid, _latid, _ztqid;

		nc_inq_varid(_ncid, "X", &_lonid);
		nc_inq_varid(_ncid, "Y", &_latid);
		nc_inq_varid(_ncid, "height", &_ztqid);

		nc_get_var(_ncid, _lonid,&lonValues[0]);
		nc_get_var(_ncid, _latid,&latValues[0]);
		nc_get_var(_ncid, _ztqid,&heiValues[0]);
		nc_get_var(_ncid, _varid,&var1Values[0]);

		float interval = 1000.0f;

		float minlon = _OriginX;
		float minlat = _OriginY;

		float w_meters = mDimX * unit;
		float h_meters = mDimY * unit;

		float h = height_in_degree(h_meters);
		float w = width_in_degree(minlat - 0.5 * h, w_meters);//用底的纬度来算，所以减去一半高度

		float maxzqt = heiValues[mDimZ - 1];

		int imageS = mDimX;
		int imageT = mDimY;
		int imageR = mDimZ;

		CVector2 center(minlon, minlat);

		float d_meters = maxzqt * unit;

		CVector3 scale;
		scale.x = w_meters / imageS;
		scale.y = h_meters / imageT;
		scale.z = d_meters / imageR;

		osg::ref_ptr<osg::Image> image = _CreateVolumeImage(imageS, imageT, imageR);

		long offset = 0;
		for (long z = 0; z < imageR; z++)
		{
			for (long y = 0; y < imageT; y++)
			{
				for (long x = 0; x < imageS; x++, offset++)
				{
					long idx = offset;
					float v = var1Values[idx];

					if (!Math::isNaN(v) && !Math::RealEqual(v, missing_value) && !Math::RealEqual(v, fill_value))
					{
						osg::Vec4ub color = _EncodeValue(v);

						unsigned char* data = (unsigned char*)image->data(x, y, z);
						data[0] = color[0];
						data[1] = color[1];

						osg::Vec2 normal2D(0.0, 0.0);
						if (y > 0 && x > 0 && y < imageT - 1 && x < imageS - 1)
						{
							float px = var1Values[idx + 1];
							float nx = var1Values[idx - 1];
							float py = var1Values[idx + imageS];
							float ny = var1Values[idx - imageS];
							CORRECT(px);
							CORRECT(nx);
							CORRECT(py);
							CORRECT(ny);
							normal2D = computeNormal(v, px, nx, px, ny);
						}

						data[2] = normal2D.x();
						data[3] = normal2D.y();
					}
				}
			}
		}

		setPosition(center);
		setMeter(osg::Vec3d(w_meters, h_meters, d_meters));
		setUint8Image(image);

		return true;
	}
}}